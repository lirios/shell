// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtGui/QIcon>

#include "notificationsadaptor.h"
#include "notificationsimagesstorage.h"
#include "notificationsimageprovider.h"

NotificationsImageProvider::NotificationsImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap NotificationsImageProvider::requestPixmap(const QString &id, QSize *realSize, const QSize &requestedSize)
{
    // Sanitize requested size
    QSize size(requestedSize);
    if (size.width() < 1)
        size.setWidth(1);
    if (size.height() < 1)
        size.setHeight(1);

    // Return real size
    if (realSize)
        *realSize = size;

    // Convert identifier (the second part is just a timestamp to make sure Image
    // is always reloaded)
    QString realId = id.split(QLatin1Char('/'), Qt::SkipEmptyParts).at(0);
    bool ok = false;
    int notificationId = realId.toInt(&ok);
    if (!ok)
        return QPixmap();

    // Get image from the daemon
    NotificationImage *image = NotificationsImagesStorage::instance()->get(notificationId);
    if (!image)
        return QPixmap();

    // Pixmap
    if (!image->image.isNull() && realSize) {
        *realSize = image->image.size();
        return image->image;
    }

    // Icon
    if (!image->iconName.isEmpty()) {
        QIcon icon = QIcon::fromTheme(image->iconName);
        if (!icon.isNull())
            return icon.pixmap(size);
    }

    // Application icon
    if (!image->entryIconName.isEmpty()) {
        QIcon icon = QIcon::fromTheme(image->entryIconName);
        if (!icon.isNull())
            return icon.pixmap(size);
    }

    // Default icon
    return QIcon::fromTheme(QStringLiteral("dialog-information")).pixmap(size);
}
