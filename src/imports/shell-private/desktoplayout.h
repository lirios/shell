// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DESKTOPLAYOUT_H
#define DESKTOPLAYOUT_H

#include <LiriAuroraCompositor/WaylandSurfaceLayout>

using namespace Aurora::Compositor;

class DesktopLayout : public WaylandSurfaceLayout
{
    Q_OBJECT
public:
    enum SurfaceRole {
        NoRole = 0,
        NotificationRole,
        OsdRole,
        FullscreenRole,
        WindowSwitcherRole,
        ModalOverlayRole,
        DialogRole,
        LockscreenRole,
    };
    Q_ENUM(SurfaceRole)

    DesktopLayout(QQuickItem *parent = nullptr);

    SurfaceRole getSurfaceRole(QQuickItem *item) const;

protected:
    WaylandSurfaceLayout::Layer getLayer(QQuickItem *item) const override;
    bool sortItems(QQuickItem *left, QQuickItem *right) override;
};

#endif // DESKTOPLAYOUT_H
