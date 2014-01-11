/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQml/QQmlContext>

#include "element.h"
#include "elementfactory.h"
#include "panelview.h"
#include "shellmanager.h"

using namespace Hawaii::Shell;

const QString s_panelViewFileName = QStringLiteral("views/PanelView.qml");

PanelView::PanelView(QQmlEngine *engine, QScreen *screen)
    : QuickView(engine, new QWindow(screen))
    , m_alignment(Qt::AlignLeft)
    , m_offset(0)
    , m_thickness(48)
{
    // Set defaults
    setFormFactor(Types::Horizontal);
    setLocation(Types::BottomEdge);

    // Resize root object to view
    setResizeMode(QuickView::SizeRootObjectToView);

    // Set Wayland window type
    setWindowType();

    // React to screen changes
    connect(this, &QWindow::screenChanged,
            this, &PanelView::positionPanel);
    connect(screen, &QScreen::geometryChanged,
            this, &PanelView::positionPanel);
    connect(this, &QuickView::locationChanged,
            this, &PanelView::positionPanel);

    // Set initial geometry
    positionPanel();

    // Let QML see us
    rootContext()->setContextProperty("view", QVariant::fromValue(this));

    // Load QML source file
    QString fileName = ShellManager::instance()->shellDirectory().filePath(s_panelViewFileName);
    setSource(QUrl::fromLocalFile(fileName));

    // Debugging message
    qDebug() << "-> Created PanelView with geometry"
             << geometry();
}

Qt::Alignment PanelView::alignment() const
{
    return m_alignment;
}

void PanelView::setAlignment(Qt::Alignment alignment)
{
    if (m_alignment != alignment) {
        m_alignment = alignment;
        Q_EMIT alignmentChanged();
    }
}

int PanelView::offset() const
{
    return m_offset;
}

void PanelView::setOffset(int value)
{
    if (m_offset != value) {
        m_offset = value;
        positionPanel();
        Q_EMIT offsetChanged();
    }
}

int PanelView::thickness() const
{
    return m_thickness;
}

void PanelView::setThickness(int value)
{
    if (m_thickness != value) {
        m_thickness = value;
        positionPanel();
        Q_EMIT thicknessChanged();
    }
}

QStringList PanelView::elements() const
{
    return m_elements;
}

void PanelView::addElement(const QString &name)
{
    if (m_elements.contains(name)) {
        m_elements.append(name);
        Q_EMIT elementsChanged();
    }

    Hawaii::Shell::Element *element = ElementFactory::createElement(name);
    Q_EMIT elementAdded(element);
}

void PanelView::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellSurfaceInterface()->set_panel(output, surface);
}

void PanelView::setSurfacePosition(const QPoint &pt)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_position(surface, pt.x(), pt.y());
}

void PanelView::positionPanel()
{
    QRect geometry = screen()->geometry();

    if (formFactor() == Types::Horizontal) {
        setWidth(geometry.width());
        setHeight(m_thickness);
    } else {
        setWidth(m_thickness);
        setHeight(geometry.height());
    }

    switch (location()) {
    case Types::LeftEdge:
        switch (m_alignment) {
        case Qt::AlignCenter:
            setPosition(QPoint(geometry.left(), geometry.center().y()) + QPoint(0, m_offset));
            break;
        case Qt::AlignRight:
            setPosition(QPoint(geometry.bottomLeft() - QPoint(0, m_offset + size().height())));
            break;
        case Qt::AlignLeft:
        default:
            setPosition(geometry.topLeft() + QPoint(0, m_offset));
        }
        break;
    case Types::TopEdge:
        switch (m_alignment) {
        case Qt::AlignCenter:
            setPosition(QPoint(geometry.center().x(), geometry.top()) + QPoint(m_offset - size().width()/2, 0));
            break;
        case Qt::AlignRight:
            setPosition(geometry.topRight() - QPoint(m_offset + size().width(), 0));
            break;
        case Qt::AlignLeft:
        default:
            setPosition(geometry.topLeft() + QPoint(m_offset, 0));
        }
        break;
    case Types::RightEdge:
        switch (m_alignment) {
        case Qt::AlignCenter:
            setPosition(QPoint(geometry.right(), geometry.center().y()) - QPoint(width(), 0) + QPoint(0, m_offset - height()/2));
            break;
        case Qt::AlignRight:
            setPosition(geometry.bottomRight() - QPoint(width(), 0) - QPoint(0, m_offset + height()));
            break;
        case Qt::AlignLeft:
        default:
            setPosition(geometry.topRight() - QPoint(width(), 0) + QPoint(0, m_offset));
        }
        break;
    case Types::BottomEdge:
        switch (m_alignment) {
        case Qt::AlignCenter:
            setPosition(QPoint(geometry.center().x(), geometry.bottom()) + QPoint(m_offset - size().width()/2, 1));
            break;
        case Qt::AlignRight:
            setPosition(geometry.bottomRight() - QPoint(0, height()) - QPoint(m_offset + width(), 1));
            break;
        case Qt::AlignLeft:
        default:
            setPosition(geometry.bottomLeft() - QPoint(0, height()) + QPoint(m_offset, 1));
        }
        break;
    default:
        break;
    }

    setSurfacePosition(position());
}

#include "moc_panelview.cpp"
