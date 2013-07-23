/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtGui/QIcon>

#include "applicationiconprovider.h"

ApplicationIconProvider::ApplicationIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ApplicationIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    int pos = id.lastIndexOf('/');
    QString iconName = id.right(id.length() - pos);

    int width = requestedSize.width();

    if (size)
        *size = requestedSize;

    if (!QIcon::hasThemeIcon(iconName)) {
        // Icon name is not available in current theme, we have to be creative...
        QString path = id;

        // Icon might be an absolute path
        if (!QFile::exists(path))
            path = QStandardPaths::locate(QStandardPaths::DataLocation,
                                          QStringLiteral("/pixmaps/%1.png").arg(id));

        // If it's not then we search in DATADIR/pixmaps
        if (QFile::exists(path)) {
            QPixmap pixmap = QPixmap(path).scaled(width, width, Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation);
            if (!pixmap.isNull())
                return pixmap;
        }

        // Otherwise we fallback to application-x-executable
        return QIcon::fromTheme(QStringLiteral("application-x-executable")).pixmap(width);
    }

    return QIcon::fromTheme(iconName).pixmap(width);
}
