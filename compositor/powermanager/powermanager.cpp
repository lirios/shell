/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtDBus/QDBusConnectionInterface>

#include "powermanager.h"
#include "systemdpowerbackend.h"
#include "upowerpowerbackend.h"

PowerManager::PowerManager(QObject *parent)
    : QObject(parent)
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();

    if (interface->isServiceRegistered(SystemdPowerBackend::service()))
        m_backends.append(new SystemdPowerBackend());

    if (interface->isServiceRegistered(UPowerPowerBackend::service()))
        m_backends.append(new UPowerPowerBackend());

    connect(interface, &QDBusConnectionInterface::serviceRegistered,
            this, &PowerManager::serviceRegistered);
    connect(interface, &QDBusConnectionInterface::serviceUnregistered,
            this, &PowerManager::serviceUnregistered);
}

PowerManager::~PowerManager()
{
    while (!m_backends.isEmpty())
        m_backends.takeFirst()->deleteLater();
}

PowerManager::Capabilities PowerManager::capabilities() const
{
    PowerManager::Capabilities caps = PowerManager::None;

    foreach (PowerManagerBackend *backend, m_backends)
        caps |= backend->capabilities();

    return caps;
}

void PowerManager::powerOff()
{
    foreach (PowerManagerBackend *backend, m_backends) {
        if (backend->capabilities() & PowerManager::PowerOff) {
            backend->powerOff();
            return;
        }
    }
}

void PowerManager::restart()
{
    foreach (PowerManagerBackend *backend, m_backends) {
        if (backend->capabilities() & PowerManager::Restart) {
            backend->restart();
            return;
        }
    }
}

void PowerManager::suspend()
{
    foreach (PowerManagerBackend *backend, m_backends) {
        if (backend->capabilities() & PowerManager::Suspend) {
            backend->suspend();
            return;
        }
    }
}

void PowerManager::hibernate()
{
    foreach (PowerManagerBackend *backend, m_backends) {
        if (backend->capabilities() & PowerManager::Hibernate) {
            backend->hibernate();
            return;
        }
    }
}

void PowerManager::hybridSleep()
{
    foreach (PowerManagerBackend *backend, m_backends) {
        if (backend->capabilities() & PowerManager::HybridSleep) {
            backend->hybridSleep();
            return;
        }
    }
}

void PowerManager::serviceRegistered(const QString &service)
{
    // Just return if we already have all the backends registered
    if (m_backends.size() == 2)
        return;

    // Otherwise add the most appropriate backend
    if (service == SystemdPowerBackend::service()) {
        m_backends.append(new SystemdPowerBackend());
        Q_EMIT capabilitiesChanged();
    } else if (service == UPowerPowerBackend::service()) {
        m_backends.append(new UPowerPowerBackend());
        Q_EMIT capabilitiesChanged();
    }
}

void PowerManager::serviceUnregistered(const QString &service)
{
    // Just return if we don't have any backend already
    if (m_backends.size() == 0)
        return;

    // Otherwise remove the backend corresponding to the service
    for (int i = 0; i < m_backends.size(); i++) {
        PowerManagerBackend *backend = m_backends.at(i);

        if (service == SystemdPowerBackend::service() && backend->name() == QStringLiteral("systemd")) {
            delete m_backends.takeAt(i);
            Q_EMIT capabilitiesChanged();
            return;
        } else if (service == UPowerPowerBackend::service() && backend->name() == QStringLiteral("upower")) {
            delete m_backends.takeAt(i);
            Q_EMIT capabilitiesChanged();
            return;
        }
    }
}

#include "moc_powermanager.cpp"
