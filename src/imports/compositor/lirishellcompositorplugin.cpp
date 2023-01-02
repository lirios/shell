// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQmlComponent>
#include <QQmlExtensionPlugin>

#include <LiriAuroraCompositor/WaylandQuickExtension>

#include "abstracttitlebar.h"
#include "appidandicon.h"
#include "chromeitem.h"
#include "desktoplayout.h"
#include "fpscounter.h"
#include "helperlauncher.h"
#include "hotspot.h"
#include "keyeventfilter.h"
#include "quickoutput.h"
#include "screencast.h"
#include "screenmodel.h"
#include "waylandcursorgrabber.h"
#include "waylandliricolorpickerv1_p.h"
#include "waylandlirimodalv1.h"
#include "waylandlirishellv1.h"
#include "windowmousetracker.h"

namespace Aurora {

namespace Compositor {

AURORA_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(WaylandLiriColorPickerManagerV1);
AURORA_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(WaylandLiriModalManagerV1);
AURORA_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(WaylandLiriShellV1);
AURORA_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(WaylandLiriOsdV1);

class LiriShellCompositorPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell.Compositor"));
        defineModule(uri);
    }

    static void defineModule(const char *uri)
    {
        // @uri Liri.Shell.Compositor

        qmlRegisterModule(uri, 1, 0);

        qmlRegisterType<AbstractTitleBar>(uri, 1, 0, "AbstractTitleBar");
        qmlRegisterType<AppIdAndIcon>(uri, 1, 0, "AppIdAndIcon");
        qmlRegisterType<ChromeItem>(uri, 1, 0, "ChromeItem");
        qmlRegisterType<DesktopLayout>(uri, 1, 0, "DesktopLayout");
        qmlRegisterType<FpsCounter>(uri, 1, 0, "FpsCounter");
        qmlRegisterType<HelperLauncher>(uri, 1, 0, "HelperLauncher");
        qmlRegisterType<HotSpot>(uri, 1, 0, "HotSpot");
        qmlRegisterType<KeyEventFilter>(uri, 1, 0, "KeyEventFilter");
        qmlRegisterType<WaylandCursorGrabber>(uri, 1, 0, "WaylandCursorGrabber");
        qmlRegisterType<WaylandLiriColorPickerManagerV1QuickExtension>(uri, 1, 0, "LiriColorPickerManagerV1");
        qmlRegisterType<WaylandLiriModalManagerV1QuickExtension>(uri, 1, 0, "LiriModalManagerV1");
        qmlRegisterType<WaylandLiriShellV1QuickExtension>(uri, 1, 0, "LiriShellV1");
        qmlRegisterType<WaylandLiriOsdV1QuickExtension>(uri, 1, 0, "LiriOsdV1");
        qmlRegisterUncreatableType<WaylandLiriShortcutV1>(uri, 1, 0, "LiriShortcutV1", QObject::tr("Cannot create instance of LiriShortcutV1"));
        qmlRegisterType<QuickOutput>(uri, 1, 0, "WaylandOutput");
        qmlRegisterType<ScreenCast>(uri, 1, 0, "ScreenCast");
        qmlRegisterType<ScreenModel>(uri, 1, 0, "ScreenModel");
        qmlRegisterUncreatableType<ScreenMode>(uri, 1, 0, "ScreenMode",
                                               QObject::tr("Cannot create instance of ScreenMode"));
        qmlRegisterUncreatableType<ScreenItem>(uri, 1, 0, "ScreenItem",
                                               QObject::tr("Cannot create instance of ScreenItem"));
        qmlRegisterType<WindowMouseTracker>(uri, 1, 0, "WindowMouseTracker");

    }
};

} // namespace Compositor

} // namespace Aurora

#include "lirishellcompositorplugin.moc"
