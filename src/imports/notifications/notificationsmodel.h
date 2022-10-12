// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSMODEL_H
#define NOTIFICATIONSMODEL_H

#include <QAbstractListModel>

#include "notificationsserver.h"

class NotificationsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        IdRole = Qt::UserRole,
        AppNameRole,
        AppIconRole,
        IconUrlRole,
        HasIconRole,
        SummaryRole,
        BodyRole,
        ActionsRole,
        IsPersistentRole,
        ExpireTimeoutRole,
        HintsRole
    };

    explicit NotificationsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void invokeAction(uint id, const QString &actionId);
    Q_INVOKABLE void closeNotification(uint id);

private:
    QList<NotificationItem *> m_items;
};

#endif // NOTIFICATIONSMODEL_H
