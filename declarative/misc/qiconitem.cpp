/*
 *   Copyright 2011 Marco Martin <mart@kde.org>
 *   Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "qiconitem.h"

#include <QSGSimpleTextureNode>
#include <qquickwindow.h>
#include <QIcon>
#include "imagetexturescache.h"
#include "managedtexturenode.h"

Q_GLOBAL_STATIC(ImageTexturesCache, s_iconImageCache)

QIconItem::QIconItem(QQuickItem *parent)
    : QQuickItem(parent),
      m_smooth(false),
      m_state(DefaultState),
      m_changed(false)
{
    setFlag(ItemHasContents, true);
}


QIconItem::~QIconItem()
{
}

void QIconItem::setIcon(const QVariant &icon)
{
    if(icon.canConvert<QIcon>()) {
        m_icon = icon.value<QIcon>();
    } else if(icon.canConvert<QString>()) {
        m_icon = QIcon::fromTheme(icon.toString());
    } else {
        m_icon = QIcon();
    }
    m_changed = true;
    update();
}

QIcon QIconItem::icon() const
{
    return m_icon;
}

QIconItem::State QIconItem::state() const
{
    return m_state;
}

void QIconItem::setState(QIconItem::State state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    m_changed = true;
    emit stateChanged(state);
    update();
}

int QIconItem::implicitWidth() const
{
    return 32;
}

int QIconItem::implicitHeight() const
{
    return 32;
}

void QIconItem::setSmooth(const bool smooth)
{
    if (smooth == m_smooth) {
        return;
    }
    m_smooth = smooth;
    m_changed = true;
    update();
}

bool QIconItem::smooth() const
{
    return m_smooth;
}

QSGNode* QIconItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* /*data*/)
{
    if (m_changed || node == 0) {
        m_changed = false;

        ManagedTextureNode* mNode = dynamic_cast<ManagedTextureNode*>(node);
        if(!mNode) {
            delete node;
            mNode = new ManagedTextureNode;
        }

        const QSize size(width(), height());
        QIcon::Mode mode;
        switch(m_state) {
            case DefaultState:
                mode = QIcon::Normal;
                break;
            case ActiveState:
                mode = QIcon::Active;
                break;
            case DisabledState:
                mode = QIcon::Disabled;
                break;
        }

        mNode->setTexture(s_iconImageCache->loadTexture(window(), m_icon.pixmap(size, mode, QIcon::On).toImage()));
        mNode->setRect(QRect(QPoint(0,0), size));
        node = mNode;
    }

    return node;
}

void QIconItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size()) {
        m_changed = true;
        update();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}
