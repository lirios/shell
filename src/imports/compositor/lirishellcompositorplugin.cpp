// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QQmlComponent>
#include <QQmlExtensionPlugin>

#include <LiriAuroraCompositor/WaylandQuickExtension>

#include "waylandliricolorpickerv1_p.h"
#include "waylandlirimodalv1.h"
#include "waylandlirishellv1.h"

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
        qmlRegisterModule(uri, 1, 0);

        qmlRegisterType<WaylandLiriColorPickerManagerV1QuickExtension>(uri, 1, 0, "LiriColorPickerManagerV1");

        qmlRegisterType<WaylandLiriModalManagerV1QuickExtension>(uri, 1, 0, "LiriModalManagerV1");

        qmlRegisterType<WaylandLiriShellV1QuickExtension>(uri, 1, 0, "LiriShellV1");
        qmlRegisterType<WaylandLiriOsdV1QuickExtension>(uri, 1, 0, "LiriOsdV1");
        qmlRegisterUncreatableType<WaylandLiriShortcutV1>(uri, 1, 0, "LiriShortcutV1", QObject::tr("Cannot create instance of LiriShortcutV1"));
    }
};

} // namespace Compositor

} // namespace Aurora

#include "lirishellcompositorplugin.moc"
