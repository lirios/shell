/***************************************************************************
 *   Copyright (C) 2014 by Eike Hein <hein@kde.org>                   *
 *   Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>   *
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

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>

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
    m_favoriteIds[SystemEntry::LockSession] = "lock-screen";
    m_favoriteIds[SystemEntry::LogoutSession] = "logout";
    m_favoriteIds[SystemEntry::NewSession] = "switch-user";
    m_favoriteIds[SystemEntry::SuspendToRam] = "suspend";
    m_favoriteIds[SystemEntry::SuspendToDisk] = "hibernate";
    m_favoriteIds[SystemEntry::Reboot] = "reboot";
    m_favoriteIds[SystemEntry::Shutdown] = "shutdown";


    if (m_sessionInterface.canLock()) {
        m_entryList << new SystemEntry(SystemEntry::LockSession, i18n("Lock"), "system-lock-screen");
        m_capabilities << LockSession;
    }

    if (m_sessionInterface.canLogOut()) {
        m_entryList << new SystemEntry(SystemEntry::LogoutSession, i18n("Logout"), "system-log-out");
        m_capabilities << LogoutSession;
    }

    if (m_sessionInterface.canStartNewSession()) {
        m_entryList << new SystemEntry(SystemEntry::NewSession, i18n("New Session"), "system-switch-user");
        m_capabilities << NewSession;
    }

    if (m_sessionInterface.canSuspend()) {
        m_entryList << new SystemEntry(SystemEntry::SuspendToRam, i18n("Suspend"), "system-suspend");
        m_capabilities << SuspendToRam;
    }

    if (m_sessionInterface.canHibernate()) {
        m_entryList << new SystemEntry(SystemEntry::SuspendToDisk, i18n("Hibernate"), "system-suspend-hibernate");
        m_capabilities << SuspendToDisk;
    }

    if (m_sessionInterface.canRestart()) {
        m_entryList << new SystemEntry(SystemEntry::Reboot, i18n("Restart"), "system-reboot");
        m_capabilities << Reboot;
    }

    if (m_sessionInterface.canPowerOff()) {
        m_entryList << new SystemEntry(SystemEntry::Shutdown, i18n("Shutdown"), "system-shutdown");
        m_capabilities << Shutdown;
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

bool SystemModel::hasCapability(const Capabilities &capability) const
{
    return m_capabilities.indexOf(capability) != -1;
}

bool SystemModel::trigger(int row, const QString &actionId, const QVariant &argument)
{
    Q_UNUSED(actionId)
    Q_UNUSED(argument)

    if (row >= 0 && row < m_entryList.count()) {
        SystemEntry::Action action = m_entryList.at(row)->action();

        switch (action) {
            case SystemEntry::LockSession:
                m_sessionInterface.lock();
                break;
            case SystemEntry::LogoutSession:
                m_sessionInterface.logOut();
                break;
            case SystemEntry::NewSession:
                m_sessionInterface.startNewSession();
                break;
            case SystemEntry::SuspendToRam:
                m_sessionInterface.suspend();
                break;
            case SystemEntry::SuspendToDisk:
                m_sessionInterface.hibernate();
                break;
            case SystemEntry::Shutdown:
                m_sessionInterface.powerOff();
                break;
            case SystemEntry::Reboot:
                m_sessionInterface.restart();
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

