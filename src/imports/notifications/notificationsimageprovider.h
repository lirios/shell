// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSIMAGEPROVIDER_H
#define NOTIFICATIONSIMAGEPROVIDER_H

#include <QQuickImageProvider>

class NotificationsImageProvider : public QQuickImageProvider
{
public:
    explicit NotificationsImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *realSize, const QSize &requestedSize);
};

#endif // NOTIFICATIONSIMAGEPROVIDER_H
