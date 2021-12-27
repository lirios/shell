// SPDX-FileCopyrightText: 2021-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_P_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_P_H

#include <QLoggingCategory>

#include "waylandlirilockscreenv1.h"
#include "aurora-server-liri-lockscreen-unstable-v1.h"

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

Q_DECLARE_LOGGING_CATEGORY(glcLiriLockScreenV1)

class WaylandLiriScreenLockerHandleV1
        : public PrivateServer::zliri_lockscreen_handle_v1
{
public:
    WaylandLiriScreenLockerHandleV1() = default;

    WaylandLiriLockScreenV1 *manager = nullptr;

protected:
    void zliri_lockscreen_handle_v1_destroy_resource(Resource *resource) override;
    void zliri_lockscreen_handle_v1_destroy(Resource *resource) override;
    void zliri_lockscreen_handle_v1_unlock(Resource *resource) override;
};

class WaylandLiriLockScreenV1Private
        : public PrivateServer::zliri_lockscreen_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriLockScreenV1)
public:
    WaylandLiriLockScreenV1Private(WaylandLiriLockScreenV1 *self);

    static WaylandLiriLockScreenV1Private *get(WaylandLiriLockScreenV1 *self) { return self->d_func(); }

    QScopedPointer<WaylandLiriScreenLockerHandleV1> handle;

protected:
    WaylandLiriLockScreenV1 *q_ptr = nullptr;

    void zliri_lockscreen_v1_bind_resource(Resource *resource) override;
    void zliri_lockscreen_v1_destroy_resource(Resource *resource) override;
    void zliri_lockscreen_v1_destroy(Resource *resource) override;
    void zliri_lockscreen_v1_get_handle(Resource *resource, uint32_t id) override;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRILOCKSCREENV1_P_H
