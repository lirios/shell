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

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(SCREENSAVER)

class ScreenSaver : public QObject
{
    Q_OBJECT
public:
    ScreenSaver(QObject *parent = 0);
    ~ScreenSaver();

    bool registerInterface();

    bool GetActive();
    bool SetActive(bool state);

    uint GetActiveTime();
    uint GetSessionIdleTime();

    void SimulateUserActivity();

    uint Inhibit(const QString &appName, const QString &reason);
    void UnInhibit(uint cookie);

    void Lock();

    uint Throttle(const QString &appName, const QString &reason);
    void UnThrottle(uint cookie);

    static constexpr const char *interfaceName = "org.freedesktop.ScreenSaver";
    static constexpr const char *objectPath = "/org/freedesktop/ScreenSaver";

Q_SIGNALS:
    void ActiveChanged(bool in);

private:
    bool m_active;
};

#endif // SCREENSAVER_H
