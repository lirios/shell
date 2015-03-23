/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QtCore/QObject>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>

class Applications;
class Windows;

class ApplicationManager : public QObject
{
    Q_OBJECT
public:
    ApplicationManager(QObject *parent = 0);
    ~ApplicationManager();

    void quit(const QString &appId);

Q_SIGNALS:
    void registered(const QString &appId, pid_t pid);
    void unregistered(const QString &appId, pid_t pid_t);

    void focused(const QString &appId);
    void unfocused(const QString &appId);

private:
    QThread *m_connectionThread;
    KWayland::Client::ConnectionThread *m_connectionThreadObject;
    KWayland::Client::EventQueue *m_eventQueue;
    Applications *m_apps;
    Windows *m_windows;
    quint32 m_appsName;
    quint32 m_windowsName;

    void initializeConnection();
    void setupRegistry(KWayland::Client::Registry *registry);
};

#endif // APPLICATIONMANAGER_H
