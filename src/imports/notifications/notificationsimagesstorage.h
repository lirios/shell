// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSIMAGESSTORAGE_H
#define NOTIFICATIONSIMAGESSTORAGE_H

#include <QHash>
#include <QPixmap>

struct NotificationImage {
    QPixmap image;
    QString iconName;
    QString entryIconName;
};

class NotificationsImagesStorage
{
public:
    NotificationsImagesStorage();
    ~NotificationsImagesStorage();

    NotificationImage *get(uint id) const;

    void add(uint id, NotificationImage *image);
    void remove(uint id);

    static NotificationsImagesStorage *instance();

private:
    QHash<uint, NotificationImage *> m_images;
};

#endif // NOTIFICATIONSIMAGESSTORAGE_H
