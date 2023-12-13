// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QObject>
#include <Qt6AccountsService/UsersModel>

class UsersModel : public QtAccountsService::UsersModel
{
    Q_OBJECT
public:
    enum CustomRoles {
        BackgroundModeRole = QtAccountsService::UsersModel::LanguageRole + 1,
        PictureUrlRole,
        FillModeRole,
        PrimaryColorRole,
        SecondaryColorRole
    };
    Q_ENUM(CustomRoles)

    explicit UsersModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;

    QVariant data(const QModelIndex &index, int role) const;
};
