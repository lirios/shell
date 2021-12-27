// SPDX-FileCopyrightText: 2020-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_H

#include <LiriAuroraCompositor/WaylandCompositorExtension>

using namespace Aurora::Compositor;

class WaylandLiriModalManagerV1Private;

class WaylandLiriModalManagerV1
        : public WaylandCompositorExtensionTemplate<WaylandLiriModalManagerV1>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandLiriModalManagerV1)
    Q_PROPERTY(bool grabbed READ isGrabbed NOTIFY grabbedChanged)
public:
    WaylandLiriModalManagerV1();
    WaylandLiriModalManagerV1(WaylandCompositor *compositor);
    ~WaylandLiriModalManagerV1();

    void initialize() override;

    bool isGrabbed() const;

    Q_INVOKABLE void sendDone();

    static const wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void grabbedChanged();

private:
    QScopedPointer<WaylandLiriModalManagerV1Private> const d_ptr;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_H
