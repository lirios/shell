/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef APPINTERFACE_H
#define APPINTERFACE_H

#include <QtCore/QObject>
#include <QtGui/QScreen>
#if 0
#include <QtQml/QQmlListProperty>
#endif

class AppInterfacePrivate;

class AppInterface : public QObject
{
    Q_OBJECT
#if 0
    Q_PRIVATE_PROPERTY(d_ptr, QQmlListProperty<QScreen> screens READ screens NOTIFY screensChanged)
#else
    Q_PROPERTY(int screenCount READ screenCount)
#endif
public:
    explicit AppInterface(QObject *parent = 0);
    virtual ~AppInterface();

    int screenCount() const;

Q_SIGNALS:
    void print(const QString &text);
    void screensChanged();

private:
    Q_DECLARE_PRIVATE(AppInterface)
    AppInterfacePrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_ptr, void _q_screenAdded(QScreen *screen))
    Q_PRIVATE_SLOT(d_ptr, void _q_screenDestroyed(QObject *object))
};

#endif // APPINTERFACE_H
