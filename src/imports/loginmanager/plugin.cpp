// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml/QtQml>

#include "usersmodel.h"

class LoginManagerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.LoginManager
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.LoginManager"));

        qmlRegisterType<UsersModel>(uri, 1, 0, "UsersModel");
    }
};

#include "plugin.moc"
