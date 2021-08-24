// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QCursor>
#include <QSGSimpleRectNode>

#include "abstracttitlebar.h"

AbstractTitleBar::AbstractTitleBar(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

QColor AbstractTitleBar::color() const
{
    return m_color;
}

void AbstractTitleBar::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    Q_EMIT colorChanged();
}

QQuickItem *AbstractTitleBar::dragTarget() const
{
    return m_dragTarget;
}

void AbstractTitleBar::setDragTarget(QQuickItem *dragTarget)
{
    if (m_dragTarget == dragTarget)
        return;

    m_dragTarget = dragTarget;
    Q_EMIT dragTargetChanged();
}

bool AbstractTitleBar::isMoving() const
{
    return m_isMoving;
}

void AbstractTitleBar::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton:
        Q_EMIT activationRequested();
        break;
    case Qt::RightButton:
        Q_EMIT windowMenuRequested(event->localPos());
        break;
    default:
        break;
    }

    event->accept();
}

void AbstractTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragTarget) {
        QQuickItem::mouseMoveEvent(event);
        return;
    }

    if (!event->buttons().testFlag(Qt::LeftButton)) {
        QQuickItem::mouseMoveEvent(event);
        return;
    }

    if (!m_isMoving) {
        m_isMoving = true;
        m_initialized = false;
        Q_EMIT movingChanged();
    }

    if (!m_initialized) {
        m_initialOffset = m_dragTarget->mapFromItem(nullptr, event->windowPos());
        m_initialized = true;
    }

    if (!m_dragTarget->parentItem())
        return;

    setCursor(QCursor(Qt::ClosedHandCursor));

    QPointF parentPos = m_dragTarget->parentItem()->mapFromItem(nullptr, event->windowPos());
    m_dragTarget->setPosition(parentPos - m_initialOffset);
}

void AbstractTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        setCursor(QCursor(Qt::ArrowCursor));

    if (m_isMoving) {
        m_isMoving = false;;
        Q_EMIT movingChanged();
    }
}

void AbstractTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_EMIT maximizeClicked();

    event->accept();
}

QSGNode *AbstractTitleBar::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    QSGSimpleRectNode *node = static_cast<QSGSimpleRectNode *>(oldNode);
    if (!node) {
        node = new QSGSimpleRectNode();
        node->setColor(m_color);
    }
    node->setRect(boundingRect());
    return node;
}
