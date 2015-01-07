/***************************************************************************
 *   Copyright (C) 2014 by Eike Hein <hein@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "systemmodel.h"
#include "actionlist.h"
#include "powermanager/powermanager.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>

#include <KAuthorized>
#include <KLocalizedString>
#include <Solid/PowerManagement>

SystemEntry::SystemEntry(SystemEntry::Action action, const QString &name, const QString &icon)
{
    m_action = action;
    m_name = name;
    m_icon = QIcon::fromTheme(icon);
}

SystemModel::SystemModel(QObject *parent) : AbstractModel(parent)
{
    m_powerManager = new PowerManager(this);

    m_favoriteIds[SystemEntry::LockSession] = "lock-screen";
    m_favoriteIds[SystemEntry::LogoutSession] = "logout";
    m_favoriteIds[SystemEntry::NewSession] = "switch-user";
    m_favoriteIds[SystemEntry::SuspendToRam] = "suspend";
    m_favoriteIds[SystemEntry::SuspendToDisk] = "hibernate";
    m_favoriteIds[SystemEntry::Reboot] = "reboot";
    m_favoriteIds[SystemEntry::Shutdown] = "shutdown";

    if (KAuthorized::authorizeKAction("lock_screen")) {
        m_entryList << new SystemEntry(SystemEntry::LockSession, i18n("Lock"), "system-lock-screen");
    }

    if (KAuthorized::authorizeKAction("logout") && KAuthorized::authorize("logout")) {
        m_entryList << new SystemEntry(SystemEntry::LogoutSession, i18n("Logout"), "system-log-out");
    }

#if 0
    if (KAuthorized::authorizeKAction("start_new_session")
        && m_displayManager.isSwitchable()
        && m_displayManager.numReserve() >= 0) {
        m_entryList << new SystemEntry(SystemEntry::NewSession, i18n("New Session"), "system-switch-user");
    }
#endif

    if (m_powerManager->capabilities() & PowerManager::Suspend) {
        m_entryList << new SystemEntry(SystemEntry::SuspendToRam, i18n("Suspend"), "system-suspend");
    }

    if (m_powerManager->capabilities() & PowerManager::Hibernate) {
        m_entryList << new SystemEntry(SystemEntry::SuspendToDisk, i18n("Hibernate"), "system-suspend-hibernate");
    }

    if (m_powerManager->capabilities() & PowerManager::Restart) {
        m_entryList << new SystemEntry(SystemEntry::Reboot, i18n("Restart"), "system-reboot");
    }

    if (m_powerManager->capabilities() & PowerManager::PowerOff) {
        m_entryList << new SystemEntry(SystemEntry::Shutdown, i18n("Shutdown"), "system-shutdown");
    }
}

SystemModel::~SystemModel()
{
    qDeleteAll(m_entryList);
}

QVariant SystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entryList.count()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return m_entryList.at(index.row())->name();
    } else if (role == Qt::DecorationRole) {
        return m_entryList.at(index.row())->icon();
    } else if (role == Kicker::FavoriteIdRole) {
        return QVariant("sys:" + m_favoriteIds[m_entryList.at(index.row())->action()]);
    }

    return QVariant();
}

int SystemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_entryList.count();
}

bool SystemModel::trigger(int row, const QString &actionId, const QVariant &argument)
{
    Q_UNUSED(actionId)
    Q_UNUSED(argument)

    if (row >= 0 && row < m_entryList.count()) {
        SystemEntry::Action action = m_entryList.at(row)->action();

        switch (action) {
            case SystemEntry::LockSession:
            {
                QDBusConnection bus = QDBusConnection::sessionBus();
                QDBusInterface interface("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", bus);
                interface.asyncCall("Lock");
                break;
            }
            case SystemEntry::LogoutSession:
#if 0
                KWorkSpace::requestShutDown(KWorkSpace::ShutdownConfirmDefault, KWorkSpace::ShutdownTypeNone);
#endif
                break;
            case SystemEntry::NewSession:
#if 0
            {
                QDBusConnection bus = QDBusConnection::sessionBus();
                QDBusInterface interface("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver", bus);
                interface.asyncCall("Lock");
                m_displayManager.startReserve();
                break;
            };
#else
            break;
#endif
            case SystemEntry::SuspendToRam:
                m_powerManager->suspend();
                break;
            case SystemEntry::SuspendToDisk:
                m_powerManager->hibernate();
                break;
            case SystemEntry::Shutdown:
                m_powerManager->powerOff();
                break;
            case SystemEntry::Reboot:
                m_powerManager->restart();
                break;
        }

        return true;
    }

    return false;
}

int SystemModel::rowForFavoriteId(const QString& favoriteId)
{
    const SystemEntry::Action action = m_favoriteIds.key(favoriteId);

    for (int i = 0; i < m_entryList.count(); ++i) {
        if (action == m_entryList.at(i)->action()) {
            return i;
        }
    }

    return -1;
}

