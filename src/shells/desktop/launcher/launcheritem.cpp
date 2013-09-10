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

#include <QtGui/QDesktopServices>

#include "launcheritem.h"
#include "launcheritem_p.h"

/*
 * LauncherItemPrivate
 */

LauncherItemPrivate::LauncherItemPrivate()
    : appInfo(0)
    , isActive(false)
    , isRunning(false)
    , isUrgent(false)
    , isDraggable(true)
    , isEditable(false)
    , isRemovable(true)
    , isCounterVisible(false)
    , counter(0)
{
}

LauncherItemPrivate::~LauncherItemPrivate()
{
    delete appInfo;
}

/*
 * LauncherItem
 */

LauncherItem::LauncherItem(const QString &fileName)
    : QObject()
    , d_ptr(new LauncherItemPrivate())
{
    d_ptr->type = LauncherItem::ApplicationItem;
    d_ptr->appInfo = new AppInfo();
    d_ptr->appInfo->load(fileName);

    connect(d_ptr->appInfo, SIGNAL(nameChanged(QString)),
            this, SIGNAL(labelChanged(QString)));
    connect(d_ptr->appInfo, SIGNAL(iconNameChanged(QString)),
            this, SIGNAL(iconNameChanged(QString)));
}

LauncherItem::LauncherItem(const QUrl &url)
    : QObject()
    , d_ptr(new LauncherItemPrivate())
{
    d_ptr->type = LauncherItem::UrlItem;
    d_ptr->url = url;
    d_ptr->isEditable = true;
}

LauncherItem::~LauncherItem()
{
    delete d_ptr;
}

LauncherItem::Type LauncherItem::type() const
{
    Q_D(const LauncherItem);
    return d->type;
}

QString LauncherItem::label() const
{
    Q_D(const LauncherItem);

    if (d->type == LauncherItem::ApplicationItem)
        return d->appInfo->name();
    return d->url.toString();
}

QString LauncherItem::iconName() const
{
    Q_D(const LauncherItem);

    if (d->type == LauncherItem::ApplicationItem)
        return d->appInfo->iconName();
    return QStringLiteral("text-html");
}

QUrl LauncherItem::url() const
{
    Q_D(const LauncherItem);

    if (d->type == LauncherItem::UrlItem)
        return d->url;
    return QUrl::fromLocalFile(d->appInfo->fileName());
}

bool LauncherItem::isActive() const
{
    Q_D(const LauncherItem);
    return d->isActive;
}

bool LauncherItem::isRunning() const
{
    Q_D(const LauncherItem);
    return d->isRunning;
}

bool LauncherItem::isUrgent() const
{
    Q_D(const LauncherItem);
    return d->isUrgent;
}

bool LauncherItem::isDraggable() const
{
    Q_D(const LauncherItem);
    return d->isDraggable;
}

bool LauncherItem::isEditable() const
{
    Q_D(const LauncherItem);
    return d->isEditable;
}

bool LauncherItem::isRemovable() const
{
    Q_D(const LauncherItem);
    return d->isRemovable;
}

bool LauncherItem::isCounterVisible() const
{
    Q_D(const LauncherItem);
    return d->isCounterVisible;
}

int LauncherItem::counter() const
{
    Q_D(const LauncherItem);
    return d->counter;
}

void LauncherItem::activate()
{
    Q_D(LauncherItem);

    // We don't know yet how to activate, so we just return
    // if this item is already running
    if (d->isRunning)
        return;

    // FIXME: We always relaunch URLs for now
    if (d->type == LauncherItem::UrlItem) {
        QDesktopServices::openUrl(d->url);
        d->isRunning = true;
        Q_EMIT runningChanged(true);
        return;
    }

    if (d->appInfo->startDetached(QStringList())) {
        d->isRunning = true;
        Q_EMIT runningChanged(true);
    }
}

void LauncherItem::launchNewInstance()
{
    Q_D(LauncherItem);

    if (d->type == LauncherItem::ApplicationItem)
        d->appInfo->startDetached(QStringList());
    else
        QDesktopServices::openUrl(d->url);
}

#include "moc_launcheritem.cpp"
