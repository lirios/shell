// SPDX-FileCopyrightText: 2020-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_P_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_P_H

#include <QLoggingCategory>

#include "waylandlirimodalv1.h"
#include "aurora-server-liri-shell-unstable-v1.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

Q_DECLARE_LOGGING_CATEGORY(gLcWaylandLiriModalV1)

class WaylandLiriModalV1
        : public PrivateServer::zliri_modal_v1
{
public:
    WaylandLiriModalV1() = default;

    WaylandLiriModalManagerV1Private *manager = nullptr;

protected:
    void zliri_modal_v1_destroy_resource(Resource *resource) override;
    void zliri_modal_v1_destroy(Resource *resource) override;
};

class WaylandLiriModalManagerV1Private
        : public PrivateServer::zliri_modal_manager_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriModalManagerV1)
public:
    explicit WaylandLiriModalManagerV1Private(WaylandLiriModalManagerV1 *self);

    void removeGrab(WaylandLiriModalV1 *grab);

    QVector<WaylandLiriModalV1 *> grabs;

protected:
    WaylandLiriModalManagerV1 *q_ptr = nullptr;

    void zliri_modal_manager_v1_grab(Resource *resource, uint32_t id,
                                     struct ::wl_resource *seatResource) override;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRIMODALV1_P_H
