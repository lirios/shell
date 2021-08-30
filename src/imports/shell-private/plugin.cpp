// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml>

#ifndef QT_WAYLAND_COMPOSITOR_QUICK
#  define QT_WAYLAND_COMPOSITOR_QUICK
#endif

#include "abstracttitlebar.h"
#include "appidandicon.h"
#include "dbus/multimediakeysserver.h"
#include "dbus/osdserver.h"
#include "chromeitem.h"
#include "desktoplayout.h"
#include "fpscounter.h"
#include "hotspot.h"
#include "keyeventfilter.h"
#include "quickoutput.h"
#include "screencast.h"
#include "screenmodel.h"
#include "waylandcursorgrabber.h"
#include "windowmousetracker.h"

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

        // Components

        qmlRegisterType<AbstractTitleBar>(uri, versionMajor, versionMinor, "AbstractTitleBar");
        qmlRegisterType<AppIdAndIcon>(uri, versionMajor, versionMinor, "AppIdAndIcon");
        qmlRegisterType<ChromeItem>(uri, versionMajor, versionMinor, "ChromeItem");
        qmlRegisterType<DesktopLayout>(uri, versionMajor, versionMinor, "DesktopLayout");
        qmlRegisterType<FpsCounter>(uri, versionMajor, versionMinor, "FpsCounter");
        qmlRegisterType<HotSpot>(uri, versionMajor, versionMinor, "HotSpot");
        qmlRegisterType<KeyEventFilter>(uri, versionMajor, versionMinor, "KeyEventFilter");
        qmlRegisterType<QuickOutput>(uri, versionMajor, versionMinor, "WaylandOutput");
        qmlRegisterType<ScreenCast>(uri, versionMajor, versionMinor, "ScreenCast");
        qmlRegisterType<ScreenModel>(uri, versionMajor, versionMinor, "ScreenModel");
        qmlRegisterUncreatableType<ScreenMode>(uri, versionMajor, versionMinor, "ScreenMode",
                                               QStringLiteral("Cannot create instance of ScreenMode"));
        qmlRegisterUncreatableType<ScreenItem>(uri, versionMajor, versionMinor, "ScreenItem",
                                               QStringLiteral("Cannot create instance of ScreenItem"));
        qmlRegisterType<WaylandCursorGrabber>(uri, versionMajor, versionMinor, "WaylandCursorGrabber");
        qmlRegisterType<WindowMouseTracker>(uri, versionMajor, versionMinor, "WindowMouseTracker");

        // D-Bus interfaces

        qmlRegisterType<OsdServer>(uri, versionMajor, versionMinor, "OsdServer");
        qmlRegisterType<MultimediaKeysServer>(uri, versionMajor, versionMinor, "MultimediaKeysServer");
    }
};

#include "plugin.moc"
