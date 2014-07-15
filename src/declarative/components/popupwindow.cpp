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

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "popupwindow.h"
#include "popupwindow_p.h"

/*
 * PopupWindowPrivate
 */

PopupWindowPrivate::PopupWindowPrivate()
    : window(0)
    , x(0)
    , y(0)
    , content(0)
{
}

PopupWindowPrivate::~PopupWindowPrivate()
{
    if (window)
        window->deleteLater();
}

/*
 * PopupWindow
 */

PopupWindow::PopupWindow(QObject *parent)
    : QObject(parent)
    , d_ptr(new PopupWindowPrivate())
{
}

PopupWindow::~PopupWindow()
{
    delete d_ptr;
}

QQuickItem *PopupWindow::contentItem() const
{
    Q_D(const PopupWindow);
    return d->content;
}

void PopupWindow::setContentItem(QQuickItem *item)
{
    Q_D(PopupWindow);

    if (d->content != item) {
        d->content = item;
        connect(d->content, SIGNAL(widthChanged()),
                this, SIGNAL(widthChanged()));
        connect(d->content, SIGNAL(heightChanged()),
                this, SIGNAL(heightChanged()));
        Q_EMIT contentChanged();
    }
}

qreal PopupWindow::x() const
{
    Q_D(const PopupWindow);
    return d->x;
}

void PopupWindow::setX(qreal x)
{
    Q_D(PopupWindow);

    if (d->x != x) {
        d->x = x;
        Q_EMIT xChanged();
    }
}

qreal PopupWindow::y() const
{
    Q_D(const PopupWindow);
    return d->y;
}

void PopupWindow::setY(qreal y)
{
    Q_D(PopupWindow);

    if (d->y != y) {
        d->y = y;
        Q_EMIT yChanged();
    }
}

qreal PopupWindow::width() const
{
    Q_D(const PopupWindow);

    if (!d->content)
        return 0;
    return d->content->width();
}

void PopupWindow::setWidth(qreal width)
{
    Q_D(PopupWindow);

    if (!d->content)
        return;
    d->content->setWidth(width);
}

qreal PopupWindow::height() const
{
    Q_D(const PopupWindow);

    if (!d->content)
        return 0;
    return d->content->height();
}

void PopupWindow::setHeight(qreal height)
{
    Q_D(PopupWindow);

    if (!d->content)
        return;
    d->content->setHeight(height);
}

bool PopupWindow::isVisible() const
{
    Q_D(const PopupWindow);
    return d->window && d->window->isVisible() ? true : false;
}

void PopupWindow::setVisible(bool value)
{
    QMetaObject::invokeMethod(this, value ? "show" : "hide", Qt::QueuedConnection);
}

void PopupWindow::show()
{
    Q_D(PopupWindow);

    // Just return if it's already visible or there's no content item
    if (isVisible() || !d->content)
        return;

    // Create the window
    if (!d->window) {
        d->window = new PopupQuickWindow(this);
        connect(d->window, SIGNAL(visibleChanged(bool)),
                this, SIGNAL(visibleChanged()));
    }

    // Set window geometry according to content
    d->window->setX(qRound(d->x));
    d->window->setY(qRound(d->y));
    d->window->setWidth(d->content->width());
    d->window->setHeight(d->content->height());

    // Reparent content item
    d->content->setParentItem(d->window->contentItem());

    // Show the window and grab input
    QMetaObject::invokeMethod(d->window, "show",
                              Qt::QueuedConnection);
    QMetaObject::invokeMethod(d->window, "setWindowType",
                              Qt::QueuedConnection);
}

void PopupWindow::hide()
{
    close();
}

void PopupWindow::close()
{
    Q_D(PopupWindow);

    if (isVisible()) {
        // Destroy the window
        d->window->deleteLater();
        d->window = 0;
        Q_EMIT visibleChanged();
    }
}

#include "moc_popupwindow.cpp"
