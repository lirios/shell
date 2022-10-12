// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2008 Dmitry Suzdalev <dimsuz@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSIMAGEUTILS_H
#define NOTIFICATIONSIMAGEUTILS_H

#include <QtDBus/QDBusArgument>
#include <QtGui/QImage>

QImage decodeImageHint(const QDBusArgument &arg);

#endif // NOTIFICATIONSIMAGEUTILS_H
