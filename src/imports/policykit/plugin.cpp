// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml/QtQml>

#include "policykitagent.h"

class PolicyKitPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.PolicyKit
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.PolicyKit"));

        qmlRegisterType<PolicyKitAgent>(uri, 1, 0, "PolicyKitAgent");
    }
};

#include "plugin.moc"
