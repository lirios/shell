// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "lirishellv1.h"
#include "aurora-client-liri-shell-unstable-v1.h"

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

class LiriShellV1Private : public PrivateClient::zliri_shell_v1
{
    Q_DECLARE_PUBLIC(LiriShellV1)
public:
    LiriShellV1Private(LiriShellV1 *qq);

    static LiriShellV1Private *get(LiriShellV1 *shell) { return shell ? shell->d_func() : nullptr; }

protected:
    LiriShellV1 *q_ptr;

private:
    void zliri_shell_v1_logout_requested() override;
    void zliri_shell_v1_shutdown_requested() override;
    void zliri_shell_v1_quit() override;
};

class LiriShortcutV1Private : public PrivateClient::zliri_shortcut_v1
{
    Q_DECLARE_PUBLIC(LiriShortcutV1)
public:
    LiriShortcutV1Private(LiriShortcutV1 *self);
    ~LiriShortcutV1Private();

    LiriShellV1 *shell = nullptr;
    QString sequence;

protected:
    LiriShortcutV1 *q_ptr;

    void zliri_shortcut_v1_activated(struct ::wl_seat *seat);
};

class LiriOsdV1Private : public PrivateClient::zliri_osd_v1
{
    Q_DECLARE_PUBLIC(LiriOsdV1)
public:
    LiriOsdV1Private(LiriOsdV1 *self);

    bool initialized = false;
    LiriShellV1 *shell = nullptr;

protected:
    LiriOsdV1 *q_ptr;

    void zliri_osd_v1_text(const QString &icon_name, const QString &label) override;
    void zliri_osd_v1_progress(const QString &icon_name, uint32_t value) override;
};

} // namespace Client

} // namespace Aurora
