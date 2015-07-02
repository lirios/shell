/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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

Q_LOGGING_CATEGORY(SCREENSAVER, "hawaii.session.screensaver")

ScreenSaver::ScreenSaver(QObject *parent)
    : QObject(parent)
    , m_active(false)
{
}

ScreenSaver::~ScreenSaver()
{
}

/*
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
*/

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
    Q_UNUSED(appName)
    Q_UNUSED(reason)

    static uint cookieSeed = 0;
    int newCookie = cookieSeed++;

    //SessionManager::instance()->idleInhibit();

    return newCookie;
}

void ScreenSaver::UnInhibit(uint cookie)
{
    Q_UNUSED(cookie)

    //SessionManager::instance()->idleUninhibit();
}

void ScreenSaver::Lock()
{
    //SessionManager::instance()->setLocked(true);
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
