// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_P_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_P_H

#include <QLoggingCategory>

#include <LiriAuroraCompositor/private/aurorawaylandcompositorextension_p.h>

#include "waylandlirishellv1.h"
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

Q_DECLARE_LOGGING_CATEGORY(gLcLiriShellV1)

class LIRIAURORACOMPOSITOR_EXPORT WaylandLiriShellV1Private
        : public WaylandCompositorExtensionPrivate
        , public PrivateServer::zliri_shell_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriShellV1)
public:
    explicit WaylandLiriShellV1Private(WaylandLiriShellV1 *qq);

    static WaylandLiriShellV1Private *get(WaylandLiriShellV1 *shell);

protected:
    void zliri_shell_v1_bind_resource(Resource *r) override;
    void zliri_shell_v1_bind_shortcut(Resource *resource, uint32_t id, const QString &sequence) override;
    void zliri_shell_v1_ready(Resource *resource, struct ::wl_resource *outputResource) override;
    void zliri_shell_v1_terminate(Resource *resource) override;
};

class LIRIAURORACOMPOSITOR_EXPORT WaylandLiriShortcutV1Private
        : public AuroraObjectPrivate
        , public PrivateServer::zliri_shortcut_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriShortcutV1)
public:
    explicit WaylandLiriShortcutV1Private(WaylandLiriShortcutV1 *self);

    static WaylandLiriShortcutV1Private *get(WaylandLiriShortcutV1 *self) { return self->d_func(); }

    WaylandCompositor *compositor = nullptr;
    QString sequence;

protected:
    WaylandLiriShortcutV1 *q_ptr;

    void zliri_shortcut_v1_destroy_resource(Resource *resource) override;
    void zliri_shortcut_v1_destroy(Resource *resource) override;
};

class LIRIAURORACOMPOSITOR_EXPORT WaylandLiriOsdV1Private
      : public WaylandCompositorExtensionPrivate
      , public PrivateServer::zliri_osd_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriOsdV1)
public:
    explicit WaylandLiriOsdV1Private(WaylandLiriOsdV1 *self);
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRISHELLV1_P_H
