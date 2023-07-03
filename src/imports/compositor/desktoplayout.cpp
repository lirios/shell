// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <LiriAuroraCompositor/WaylandWlrLayerShellV1>
#include <LiriAuroraCompositor/WaylandWlrLayerSurfaceItem>

#include "desktoplayout.h"

DesktopLayout::DesktopLayout(QQuickItem *parent)
    : WaylandSurfaceLayout(parent)
{
}

DesktopLayout::SurfaceRole DesktopLayout::getSurfaceRole(QQuickItem *item) const
{
    static const QHash<QString, DesktopLayout::SurfaceRole> namespaceToRole {
        { QStringLiteral("splash"), DesktopLayout::SplashRole },
        { QStringLiteral("osd"), DesktopLayout::OsdRole },
        { QStringLiteral("run-dialog"), DesktopLayout::RunDialogRole },
        { QStringLiteral("auth-dialog"), DesktopLayout::AuthDialogRole },
        { QStringLiteral("logout-dialog"), DesktopLayout::LogoutDialogRole },
        { QStringLiteral("poweroff-dialog"), DesktopLayout::PowerOffDialogRole },
        { QStringLiteral("window-switcher"), DesktopLayout::WindowSwitcherRole },
    };
    if (auto *l = qobject_cast<WaylandWlrLayerSurfaceItem *>(item))
        return namespaceToRole.value(l->layerSurface()->nameSpace().toLower(), DesktopLayout::NoRole);
    else
        return namespaceToRole.value(item->objectName().toLower(), DesktopLayout::NoRole);
}

DesktopLayout::Layer DesktopLayout::getLayer(QQuickItem *item) const
{
    static const QHash<WaylandWlrLayerShellV1::Layer, DesktopLayout::Layer> layerToIndex {
        { WaylandWlrLayerShellV1::BackgroundLayer, DesktopLayout::BackgroundLayer },
        { WaylandWlrLayerShellV1::BottomLayer, DesktopLayout::BottomLayer },
        { WaylandWlrLayerShellV1::TopLayer, DesktopLayout::TopLayer },
        { WaylandWlrLayerShellV1::OverlayLayer, DesktopLayout::OverlayLayer },
    };

    static const QMultiHash<QString, DesktopLayout::Layer> namespaceToLayer {
        { QStringLiteral("workspaces"), DesktopLayout::WindowsLayer },
        { QStringLiteral("toplayer"), DesktopLayout::TopLayer },
        { QStringLiteral("fullscreen"), DesktopLayout::FullscreenLayer },
        { QStringLiteral("auth-dialog"), DesktopLayout::DialogLayer },
        { QStringLiteral("run-dialog"), DesktopLayout::DialogLayer },
        { QStringLiteral("logout-dialog"), DesktopLayout::DialogLayer },
        { QStringLiteral("poweroff-dialog"), DesktopLayout::DialogLayer },
        { QStringLiteral("lockscreen"), DesktopLayout::LockScreenLayer },
        { QStringLiteral("window-switcher"), DesktopLayout::OverlayLayer },
    };

    if (auto *l = qobject_cast<WaylandWlrLayerSurfaceItem *>(item))
        return layerToIndex.value(l->layerSurface()->layer(), DesktopLayout::NoLayer);
    else
        return namespaceToLayer.value(item->objectName(), DesktopLayout::NoLayer);
}

bool DesktopLayout::sortItems(QQuickItem *left, QQuickItem *right)
{
    if (getLayer(left) == getLayer(right))
        return getSurfaceRole(left) < getSurfaceRole(right);
    return getLayer(left) < getLayer(right);
}
