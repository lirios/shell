// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <LiriWaylandServer/WaylandWlrLayerShellV1>
#include <LiriWaylandServer/WaylandWlrLayerSurfaceV1Item>

#include "desktoplayout.h"

DesktopLayout::DesktopLayout(QQuickItem *parent)
    : WaylandSurfaceLayout(parent)
{
}

DesktopLayout::SurfaceRole DesktopLayout::getSurfaceRole(QQuickItem *item) const
{
    static const QHash<QString, DesktopLayout::SurfaceRole> namespaceToRole {
        { QStringLiteral("notifications"), DesktopLayout::NotificationRole },
        { QStringLiteral("osd"), DesktopLayout::OsdRole },
        { QStringLiteral("fullscreen"), DesktopLayout::FullscreenRole },
        { QStringLiteral("window-switcher"), DesktopLayout::WindowSwitcherRole },
        { QStringLiteral("modal-overlay"), DesktopLayout::ModalOverlayRole },
        { QStringLiteral("dialog"), DesktopLayout::DialogRole },
        { QStringLiteral("lockscreen"), DesktopLayout::LockscreenRole },
    };
    if (auto *l = qobject_cast<WaylandWlrLayerSurfaceV1Item *>(item))
        return namespaceToRole.value(l->layerSurface()->nameSpace().toLower(), DesktopLayout::NoRole);
    else
        return namespaceToRole.value(item->objectName().toLower(), DesktopLayout::NoRole);
}

WaylandSurfaceLayout::Layer DesktopLayout::getLayer(QQuickItem *item) const
{
    static const QHash<WaylandWlrLayerShellV1::Layer, WaylandSurfaceLayout::Layer> layerToIndex {
        { WaylandWlrLayerShellV1::BackgroundLayer, WaylandSurfaceLayout::BackgroundLayer },
        { WaylandWlrLayerShellV1::BottomLayer, WaylandSurfaceLayout::BottomLayer },
        { WaylandWlrLayerShellV1::TopLayer, WaylandSurfaceLayout::TopLayer },
        { WaylandWlrLayerShellV1::OverlayLayer, WaylandSurfaceLayout::OverlayLayer },
    };

    if (auto *l = qobject_cast<WaylandWlrLayerSurfaceV1Item *>(item))
        return layerToIndex.value(l->layerSurface()->layer(), WaylandSurfaceLayout::NoLayer);
    else if (item->objectName() == QStringLiteral("workspaces"))
        return WaylandSurfaceLayout::WindowsLayer;
    else if (item->objectName() == QStringLiteral("shell"))
        return WaylandSurfaceLayout::TopLayer;
    else if (item->objectName() == QStringLiteral("fullscreen"))
        return WaylandSurfaceLayout::OverlayLayer;
    else if (item->objectName() == QStringLiteral("windowSwitcher"))
        return WaylandSurfaceLayout::OverlayLayer;
    return WaylandSurfaceLayout::NoLayer;
}

bool DesktopLayout::sortItems(QQuickItem *left, QQuickItem *right)
{
    if (getLayer(left) == getLayer(right))
        return getSurfaceRole(left) < getSurfaceRole(right);
    return getLayer(left) < getLayer(right);
}
