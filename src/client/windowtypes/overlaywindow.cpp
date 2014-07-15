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

#include "overlaywindow.h"
#include "overlaywindow_p.h"

/*
 * OverlayWindowPrivate
 */

OverlayWindowPrivate::OverlayWindowPrivate()
    : window(0)
    , content(0)
{
}

OverlayWindowPrivate::~OverlayWindowPrivate()
{
    if (window)
        window->deleteLater();
}

/*
 * OverlayWindow
 */

OverlayWindow::OverlayWindow(QObject *parent)
    : QObject(parent)
    , d_ptr(new OverlayWindowPrivate())
{
}

OverlayWindow::~OverlayWindow()
{
    delete d_ptr;
}

QQuickItem *OverlayWindow::contentItem() const
{
    Q_D(const OverlayWindow);
    return d->content;
}

void OverlayWindow::setContentItem(QQuickItem *item)
{
    Q_D(OverlayWindow);

    if (d->content != item) {
        d->content = item;
        Q_EMIT contentChanged();
    }
}

bool OverlayWindow::isVisible() const
{
    Q_D(const OverlayWindow);
    return d->window && d->window->isVisible() ? true : false;
}

void OverlayWindow::setVisible(bool value)
{
    QMetaObject::invokeMethod(this, value ? "show" : "hide", Qt::QueuedConnection);
}

void OverlayWindow::show()
{
    Q_D(OverlayWindow);

    // Just return if it's already visible or there's no content item
    if (isVisible() || !d->content)
        return;

    // Create the window
    if (!d->window) {
        d->window = new OverlayQuickWindow();
        connect(d->window, SIGNAL(rejected()), this, SIGNAL(rejected()));
    }

    // Set window size according to content size
    d->window->setWidth(d->content->width());
    d->window->setHeight(d->content->height());

    // Set screen and reparent content item
    d->window->setScreen(QGuiApplication::primaryScreen());
    d->content->setParentItem(d->window->contentItem());

    // Show the window
    d->window->show();
    Q_EMIT visibleChanged();
}

void OverlayWindow::hide()
{
    Q_D(OverlayWindow);

    if (isVisible()) {
        d->window->deleteLater();
        d->window = 0;
        Q_EMIT visibleChanged();
    }
}

#include "moc_overlaywindow.cpp"
