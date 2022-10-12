// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "notificationsmodel.h"
#include "notificationsserver.h"

NotificationsModel::NotificationsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto *server = NotificationsServer::instance();
    connect(server, &NotificationsServer::notificationAdded, this, [=](NotificationItem *item) {
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
        m_items.append(item);
        endInsertRows();
    });
    connect(server, &NotificationsServer::notificationReplaced, this, [=](NotificationItem *item) {
        if (int row = m_items.indexOf(item) >= 0) {
            auto modelIndex = index(row);
            Q_EMIT dataChanged(modelIndex, modelIndex);
        }
    });
    connect(server, &NotificationsServer::notificationRemoved, this, [=](uint id) {
        for (int index = 0; index < m_items.size(); index++) {
            auto *item = m_items.at(index);
            if (item->notificationId == id) {
                beginRemoveRows(QModelIndex(), index, index);
                m_items.removeAt(index);
                endRemoveRows();
                break;
            }
        }
    });
}

int NotificationsModel::rowCount(const QModelIndex &parent) const
{
    return m_items.size();
}

QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(IdRole, "notificationId");
    roleNames.insert(AppNameRole, "appName");
    roleNames.insert(AppIconRole, "appIcon");
    roleNames.insert(IconUrlRole, "iconUrl");
    roleNames.insert(HasIconRole, "hasIcon");
    roleNames.insert(SummaryRole, "summary");
    roleNames.insert(BodyRole, "body");
    roleNames.insert(ActionsRole, "actions");
    roleNames.insert(IsPersistentRole, "isPersistent");
    roleNames.insert(ExpireTimeoutRole, "expireTimeout");
    roleNames.insert(HintsRole, "hints");
    return roleNames;
}

QVariant NotificationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    NotificationItem *item = m_items.at(index.row());
    if (!item)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return item->summary;
    case IdRole:
        return item->notificationId;
    case AppNameRole:
        return item->appName;
    case AppIconRole:
        return item->appIcon;
    case IconUrlRole:
        return item->iconUrl;
    case HasIconRole:
        return item->hasIcon;
    case SummaryRole:
        return item->summary;
    case BodyRole:
        return item->body;
    case ActionsRole:
        return item->actions;
    case IsPersistentRole:
        return item->isPersistent;
    case ExpireTimeoutRole:
        return item->expireTimeout;
    case HintsRole:
        return item->hints;
    };

    return QVariant();
}

void NotificationsModel::invokeAction(uint id, const QString &actionId)
{
    NotificationsServer::instance()->invokeAction(id, actionId);
}

void NotificationsModel::closeNotification(uint id)
{
    NotificationsServer::instance()->closeNotification(id);
}
