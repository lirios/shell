/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDebug>
#include <QtCore/QTimer>
#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>

#include "tooltipwindow.h"

TooltipWindow::TooltipWindow(QQuickItem *parent)
    : QQuickItem(parent)
    , m_showCount(0)
    , m_content(0)
    , m_window(0)
{
    m_showTimer = new QTimer(this);
    m_showTimer->setInterval(500);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, SIGNAL(timeout()),
            this, SLOT(showWindow()));

    m_hideTimer = new QTimer(this);
    m_hideTimer->setInterval(500);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, SIGNAL(timeout()),
            this, SLOT(hideWindow()));
}

TooltipWindow::~TooltipWindow()
{
    if (m_window)
        m_window->deleteLater();
}

QQuickItem *TooltipWindow::content() const
{
    return m_content;
}

void TooltipWindow::setContent(QQuickItem *item)
{
    m_content = item;
}

void TooltipWindow::show()
{
    m_hideTimer->stop();

    if (m_window || !parentItem() || !m_content)
        return;

    QQuickWindow *window = parentItem()->window();
    if (!window)
        return;
    window->installEventFilter(this);

    if (m_showCount == 0)
        m_showTimer->start();
}

void TooltipWindow::hide()
{
    m_showTimer->stop();

    if (!m_window)
        return;

    m_window->deleteLater();
    m_window = 0;
    parentItem()->window()->removeEventFilter(this);

    m_hideTimer->start();
}

bool TooltipWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
        hide();
    return QQuickItem::eventFilter(object, event);
}

void TooltipWindow::showWindow()
{
    QQuickWindow *window = parentItem()->window();

    // Create a transient window and reparent tooltip's content
    m_window = new QQuickWindow();
    m_window->setTransientParent(window);
    m_window->setFlags(Qt::ToolTip);
    m_window->setScreen(window->screen());
    m_content->setParentItem(m_window->contentItem());

    // Parent item position
    QPointF pos = parentItem()->mapToScene(QPointF(0, 0));
    QPoint pt = window->mapToGlobal(QPoint(pos.x(), pos.y()));

    // Set window properties
    m_window->setX(pt.x());
    m_window->setY(pt.y() - window->height());
    m_window->setWidth(m_content->width());
    m_window->setHeight(m_content->height());
    m_window->setColor(Qt::transparent);

    // Show window
    QMetaObject::invokeMethod(m_window, "show", Qt::QueuedConnection);
    m_showCount++;
}

void TooltipWindow::hideWindow()
{
    m_showCount--;
}

#include "moc_tooltipwindow.cpp"
