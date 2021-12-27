// SPDX-FileCopyrightText: 2021-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_H

#include <LiriAuroraCompositor/WaylandCompositorExtension>

using namespace Aurora::Compositor;

class WaylandLiriLockScreenV1Private;

class LIRIAURORACOMPOSITOR_EXPORT WaylandLiriLockScreenV1
        : public WaylandCompositorExtensionTemplate<WaylandLiriLockScreenV1>
{
    Q_OBJECT
    Q_PROPERTY(bool clientAvailable READ isClientAvailable NOTIFY clientAvailableChanged)
    Q_DECLARE_PRIVATE(WaylandLiriLockScreenV1)
public:
    WaylandLiriLockScreenV1();
    WaylandLiriLockScreenV1(WaylandCompositor *compositor);
    ~WaylandLiriLockScreenV1();

    void initialize() override;

    bool isClientAvailable() const;

    Q_INVOKABLE void requestLock();

    static const wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void clientAvailableChanged(bool available);
    void unlocked();

private:
    QScopedPointer<WaylandLiriLockScreenV1Private> const d_ptr;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_H
