// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml>

#include "extensionsmodel.h"
#include "lirishellv1.h"
#include "multimediakeysclient.h"
#include "osdclient.h"

class ShellPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri) override
    {
        // @uri Liri.Shell
        Q_ASSERT(strcmp(uri, "Liri.Shell") == 0);

        const int versionMajor = 1;
        const int versionMinor = 0;

        qmlRegisterType<ExtensionsModel>(uri, versionMajor, versionMinor, "ExtensionsModel");
        qmlRegisterSingletonType<MultimediaKeysClient>(uri, versionMajor, versionMinor, "MultimediaKeysClient",
                                                       [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new MultimediaKeysClient();
        });
        qmlRegisterSingletonType<OsdClient>(uri, versionMajor, versionMinor, "OsdClient",
                                            [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new OsdClient();
        });

        qmlRegisterType<Aurora::Client::LiriShellV1>(uri, versionMajor, versionMinor, "LiriShellV1");
        qmlRegisterType<Aurora::Client::LiriShortcutV1>(uri, versionMajor, versionMinor, "LiriShortcutV1");
        qmlRegisterType<Aurora::Client::LiriOsdV1>(uri, versionMajor, versionMinor, "LiriOsdV1");
    }
};

#include "plugin.moc"
