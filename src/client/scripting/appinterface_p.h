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

#ifndef APPINTERFACE_P_H
#define APPINTERFACE_P_H

#include <QtGui/QScreen>
#if 0
#include <QtQml/QQmlListProperty>
#endif

class AppInterface;

class AppInterfacePrivate
{
public:
    AppInterfacePrivate(AppInterface *parent);

    void _q_screenAdded(QScreen *screen);
    void _q_screenDestroyed(QObject *object);

#if 0
    QQmlListProperty<QScreen> screens();

    static int screensCount(QQmlListProperty<QScreen> *p);
    static QScreen *screenAt(QQmlListProperty<QScreen> *p, int index);
#else
    QList<QScreen *> screens;
#endif

protected:
    Q_DECLARE_PUBLIC(AppInterface)
    AppInterface *q_ptr;
};

#endif // APPINTERFACE_P_H
