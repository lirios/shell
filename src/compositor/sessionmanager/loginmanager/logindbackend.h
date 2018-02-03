/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef LOGINDBACKEND_H
#define LOGINDBACKEND_H

#include <QLoggingCategory>

#include <LiriLogind/Logind>

#include "loginmanagerbackend.h"

Q_DECLARE_LOGGING_CATEGORY(LOGIND_BACKEND)

class SessionManager;

class LogindBackend : public LoginManagerBackend
{
    Q_OBJECT
public:
    ~LogindBackend();

    static LogindBackend *create(SessionManager *sm);

    QString name() const;

    void setIdle(bool value);

    void takeControl();
    void releaseControl();

    int takeDevice(const QString &path);
    void releaseDevice(int fd);

    void lockSession();
    void unlockSession();

    void requestLockSession();
    void requestUnlockSession();

    void locked();
    void unlocked();

    void switchToVt(quint32 vt);

private:
    LogindBackend();

    SessionManager *m_sessionManager = nullptr;
    int m_powerButtonFd = -1;
    int m_inhibitFd = -1;

    void setupInhibitors();

private Q_SLOTS:
    void handleConnectedChanged(bool connected);
    void handleInhibited(const QString &who, const QString &why, int fd);
    void handleUninhibited(int fd);
    void prepareForSleep(bool arg);
    void prepareForShutdown(bool arg);
    void devicePaused(quint32 devMajor, quint32 devMinor, const QString &type);
};

#endif // LOGINDBACKEND_H
