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
        { QStringLiteral("osd"), DesktopLayout::OsdRole },
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

    if (auto *l = qobject_cast<WaylandWlrLayerSurfaceItem *>(item))
        return layerToIndex.value(l->layerSurface()->layer(), DesktopLayout::NoLayer);
    else if (item->objectName() == QStringLiteral("workspaces"))
        return DesktopLayout::WindowsLayer;
    else if (item->objectName() == QStringLiteral("shell"))
        return DesktopLayout::TopLayer;
    else if (item->objectName() == QStringLiteral("fullscreen"))
        return DesktopLayout::FullscreenLayer;
    else if (item->objectName() == QStringLiteral("modal-overlay"))
        return DesktopLayout::ModalOverlayLayer;
    else if (item->objectName() == QStringLiteral("dialog"))
        return DesktopLayout::DialogLayer;
    else if (item->objectName() == QStringLiteral("lockscreen"))
        return DesktopLayout::LockScreenLayer;
    else if (item->objectName() == QStringLiteral("window-switcher"))
        return DesktopLayout::OverlayLayer;
    return DesktopLayout::NoLayer;
}

bool DesktopLayout::sortItems(QQuickItem *left, QQuickItem *right)
{
    if (getLayer(left) == getLayer(right))
        return getSurfaceRole(left) < getSurfaceRole(right);
    return getLayer(left) < getLayer(right);
}
