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
        OsdRole,
        RunDialogRole,
        AuthDialogRole,
        LogoutDialogRole,
        PowerOffDialogRole,
        WindowSwitcherRole,
        SplashRole,
    };
    Q_ENUM(SurfaceRole)

    enum Layer {
        NoLayer = 0,
        BackgroundLayer,
        BottomLayer,
        WindowsLayer,
        TopLayer,
        DialogLayer,
        FullscreenLayer,
        LockScreenLayer,
        OverlayLayer,
    };
    Q_ENUM(Layer);

    DesktopLayout(QQuickItem *parent = nullptr);

    SurfaceRole getSurfaceRole(QQuickItem *item) const;

protected:
    Layer getLayer(QQuickItem *item) const;
    bool sortItems(QQuickItem *left, QQuickItem *right) override;
};

#endif // DESKTOPLAYOUT_H
