/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#pragma once

#include <QtCore/QObject>
#include <QtAccountsService/UsersModel>

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
