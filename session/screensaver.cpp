/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include "screensaver.h"
#include "screensaveradaptor.h"

Q_LOGGING_CATEGORY(SCREENSAVER, "hawaii.session.screensaver")

ScreenSaver::ScreenSaver(QObject *parent)
    : QObject(parent)
    , m_active(false)
{
    new ScreenSaverAdaptor(this);
}

ScreenSaver::~ScreenSaver()
{

}

bool ScreenSaver::registerInterface()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerObject(objectPath, this)) {
        qCWarning(SCREENSAVER,
                  "Couldn't register %s D-Bus object: %s",
                  objectPath,
                  qPrintable(bus.lastError().message()));
        return false;
    }

    qCDebug(SCREENSAVER) << "Registered" << interfaceName << "D-Bus interface";
    return true;
}

bool ScreenSaver::GetActive()
{
    return m_active;
}

bool ScreenSaver::SetActive(bool state)
{
    if (state) {
        Lock();
        return true;
    }

    return false;
}

uint ScreenSaver::GetActiveTime()
{
    return 0;
}

uint ScreenSaver::GetSessionIdleTime()
{
    return 0;
}

void ScreenSaver::SimulateUserActivity()
{
}

uint ScreenSaver::Inhibit(const QString &appName, const QString &reason)
{
    // TODO:
    return 0;
}

void ScreenSaver::UnInhibit(uint cookie)
{
    // TODO:
    Q_UNUSED(cookie);
}

void ScreenSaver::Lock()
{
}

uint ScreenSaver::Throttle(const QString &appName, const QString &reason)
{
    // TODO:
    Q_UNUSED(appName);
    Q_UNUSED(reason);
    return 0;
}

void ScreenSaver::UnThrottle(uint cookie)
{
    // TODO:
    Q_UNUSED(cookie);
}

#include "moc_screensaver.cpp"
