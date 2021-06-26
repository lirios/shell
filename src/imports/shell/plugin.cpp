// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml>

#include "multimediakeysclient.h"
#include "osdclient.h"

class ShellPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.Shell
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell"));

        const int versionMajor = 1;
        const int versionMinor = 0;

        qmlRegisterSingletonType<MultimediaKeysClient>(uri, versionMajor, versionMinor, "MultimediaKeysClient",
                                                       [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new MultimediaKeysClient();
        });
        qmlRegisterSingletonType<OsdClient>(uri, versionMajor, versionMinor, "OsdClient",
                                            [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new OsdClient();
        });
    }
};

#include "plugin.moc"
