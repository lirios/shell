// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QGlobalStatic>

#include "notificationsimagesstorage.h"

Q_GLOBAL_STATIC(NotificationsImagesStorage, globalStorage)

NotificationsImagesStorage::NotificationsImagesStorage()
{
}

NotificationsImagesStorage::~NotificationsImagesStorage()
{
    qDeleteAll(m_images);
}

NotificationImage *NotificationsImagesStorage::get(uint id) const
{
    return m_images.value(id);
}

void NotificationsImagesStorage::add(uint id, NotificationImage *image)
{
    remove(id);
    m_images[id] = image;
}

void NotificationsImagesStorage::remove(uint id)
{
    if (m_images.contains(id))
        delete m_images.take(id);
}

NotificationsImagesStorage *NotificationsImagesStorage::instance()
{
    return globalStorage;
}
