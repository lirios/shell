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

#include <QtGui/QPixmapCache>

#include "notificationimageprovider.h"

NotificationImageProvider::NotificationImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

void NotificationImageProvider::insertPixmap(const QString &key, const QPixmap &pixmap)
{
    QString name = QStringLiteral("notifications-") + key;
    QPixmapCache::insert(name, pixmap);
}

void NotificationImageProvider::removePixmap(const QString &key)
{
    QString name = QStringLiteral("notifications-") + key;
    QPixmapCache::remove(name);
}

QPixmap NotificationImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString name = QStringLiteral("notifications-") + id;
    QPixmap pixmap;

    if (!QPixmapCache::find(name, &pixmap))
        return QPixmap();
    if (pixmap.isNull())
        return QPixmap();

    if (requestedSize.isValid()) {
        QPixmap result = pixmap.scaled(requestedSize, Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
        if (!result.isNull())
            pixmap = result;
    }

    if (size)
        *size = pixmap.size();

    return pixmap;
}

