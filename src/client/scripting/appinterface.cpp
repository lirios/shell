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

#include <QtGui/QGuiApplication>

#include "appinterface.h"
#include "appinterface_p.h"

/*
 * AppInterfacePrivate
 */

AppInterfacePrivate::AppInterfacePrivate(AppInterface *parent)
    : q_ptr(parent)
{
}

void AppInterfacePrivate::_q_screenAdded(QScreen *screen)
{
    Q_Q(AppInterface);

    q->connect(screen, SIGNAL(destroyed(QObject*)),
               q, SLOT(_q_screenDestroyed(QObject *)));
    Q_EMIT q->screensChanged();
}

void AppInterfacePrivate::_q_screenDestroyed(QObject *object)
{
    Q_UNUSED(object);
    Q_Q(AppInterface);
    Q_EMIT q->screensChanged();
}

#if 0
QQmlListProperty<QScreen> AppInterfacePrivate::screens()
{
    Q_Q(AppInterface);
    return QQmlListProperty<QScreen>(q, 0, screensCount, screenAt);
}

int AppInterfacePrivate::screensCount(QQmlListProperty<QScreen> *p)
{
    Q_UNUSED(p);
    return QGuiApplication::screens().size();
}

QScreen *AppInterfacePrivate::screenAt(QQmlListProperty<QScreen> *p, int index)
{
    Q_UNUSED(p);
    return QGuiApplication::screens().at(index);
}
#endif

/*
 * AppInterface
 */

AppInterface::AppInterface(QObject *parent)
    : QObject(parent)
    , d_ptr(new AppInterfacePrivate(this))
{
    connect(qApp, SIGNAL(screenAdded(QScreen*)),
            this, SLOT(_q_screenAdded(QScreen*)));
}

AppInterface::~AppInterface()
{
    delete d_ptr;
}

int AppInterface::screenCount() const
{
    return QGuiApplication::screens().size();
}

#include "moc_appinterface.cpp"
