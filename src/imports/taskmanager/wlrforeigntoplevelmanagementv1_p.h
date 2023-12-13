// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "wlrforeigntoplevelmanagementv1.h"
#include "aurora-client-wlr-foreign-toplevel-management-unstable-v1.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Aurora API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace Aurora {

namespace Client {

class WlrForeignToplevelManagerV1Private
        : public PrivateClient::zwlr_foreign_toplevel_manager_v1
{
    Q_DECLARE_PUBLIC(WlrForeignToplevelManagerV1)
public:
    explicit WlrForeignToplevelManagerV1Private(WlrForeignToplevelManagerV1 *self);

    QVector<WlrForeignToplevelHandleV1 *> handles;

protected:
    WlrForeignToplevelManagerV1 *q_ptr;

    void zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel) override;
    void zwlr_foreign_toplevel_manager_v1_finished() override;
};

class WlrForeignToplevelHandleV1Private
        : public PrivateClient::zwlr_foreign_toplevel_handle_v1
{
    Q_DECLARE_PUBLIC(WlrForeignToplevelHandleV1)
public:
    struct HandleInfo {
        QString title;
        QString appId;
        bool minimized = false;
        bool maximized = false;
        bool fullscreen = false;
        bool activated = false;
    };

    explicit WlrForeignToplevelHandleV1Private(WlrForeignToplevelHandleV1 *self);
    ~WlrForeignToplevelHandleV1Private();

    static WlrForeignToplevelHandleV1Private *get(WlrForeignToplevelHandleV1 *handle) { return handle->d_func(); }

    WlrForeignToplevelManagerV1Private *manager = nullptr;
    HandleInfo pending;
    HandleInfo effective;

protected:
    WlrForeignToplevelHandleV1 *q_ptr;

    void zwlr_foreign_toplevel_handle_v1_title(const QString &title) override;
    void zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId) override;
    void zwlr_foreign_toplevel_handle_v1_output_enter(struct ::wl_output *output) override;
    void zwlr_foreign_toplevel_handle_v1_output_leave(struct ::wl_output *output) override;
    void zwlr_foreign_toplevel_handle_v1_state(wl_array *rawData) override;
    void zwlr_foreign_toplevel_handle_v1_done() override;
    void zwlr_foreign_toplevel_handle_v1_closed() override;
    void zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent) override;
};

} // namespace Client

} // namespace Aurora
