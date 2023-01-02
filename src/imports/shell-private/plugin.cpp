// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml>

#ifndef QT_WAYLAND_COMPOSITOR_QUICK
#  define QT_WAYLAND_COMPOSITOR_QUICK
#endif

#include "dbus/multimediakeysserver.h"
#include "dbus/osdserver.h"

class ShellPrivatePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.private.shell
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.private.shell"));

        const int versionMajor = 1;
        const int versionMinor = 0;

        // D-Bus interfaces

        qmlRegisterType<OsdServer>(uri, versionMajor, versionMinor, "OsdServer");
        qmlRegisterType<MultimediaKeysServer>(uri, versionMajor, versionMinor, "MultimediaKeysServer");
    }
};

#include "plugin.moc"
