// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QSettings>
#include <QDebug>

#include <Qt5AccountsService/UserAccount>

#include "usersmodel.h"

UsersModel::UsersModel(QObject *parent)
    : QtAccountsService::UsersModel(parent)
{
}

QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles = QtAccountsService::UsersModel::roleNames();
    roles.insert(BackgroundModeRole, QByteArrayLiteral("backgroundMode"));
    roles.insert(PictureUrlRole, QByteArrayLiteral("pictureUrl"));
    roles.insert(FillModeRole, QByteArrayLiteral("fillMode"));
    roles.insert(PrimaryColorRole, QByteArrayLiteral("primaryColor"));
    roles.insert(SecondaryColorRole, QByteArrayLiteral("secondaryColor"));
    return roles;
}

QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role < BackgroundModeRole)
        return QtAccountsService::UsersModel::data(index, role);

    QtAccountsService::UserAccount *user = userAccount(index);
    if (!user)
        return QVariant();

    const QDir backgroundDir = QDir::home().absoluteFilePath(QStringLiteral(".local/share/sddm/backgrounds"));
    const QString confFileName = backgroundDir.absoluteFilePath(user->userName() + QLatin1String(".conf"));

    // Read the configuration even if the file doesn't exist,
    // in that case we'll return default values to avoid QML errors
    // of assigning null or undefined values to color properties
    QSettings conf(confFileName, QSettings::IniFormat);
    conf.beginGroup(QStringLiteral("Background"));

    switch (role) {
    case BackgroundModeRole:
        return conf.value(QStringLiteral("Mode"), QString());
    case PictureUrlRole: {
        const QString fileName = conf.value(QStringLiteral("PictureUrl"), QString()).toString();
        if (fileName.isEmpty())
            return QUrl();
        return QUrl::fromLocalFile(backgroundDir.absoluteFilePath(fileName));
    }
    case FillModeRole:
        return conf.value(QStringLiteral("FillMode"), QLatin1String("preserve-aspect-crop"));
    case PrimaryColorRole:
        return conf.value(QStringLiteral("PrimaryColor"), QLatin1String("#336699"));
    case SecondaryColorRole:
        return conf.value(QStringLiteral("SecondaryColor"), QLatin1String("#336699"));
    default:
        break;
    }

    return QVariant();
}

#include "moc_usersmodel.cpp"
