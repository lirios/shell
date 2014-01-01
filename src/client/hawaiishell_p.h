/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef HAWAIISHELL_P_H
#define HAWAIISHELL_P_H

#include <QtCore/QElapsedTimer>

#include "hawaiishell.h"
#include "qwayland-hawaii.h"
#include "qwayland-notification.h"

class QQmlComponent;

class HawaiiShellImpl : public QtWayland::wl_hawaii_shell
{
public:
    HawaiiShellImpl(HawaiiShellPrivate *parent);

protected:
    virtual void hawaii_shell_loaded();
    virtual void hawaii_shell_prepare_lock_surface();
    virtual void hawaii_shell_grab_cursor(uint32_t cursor);
    virtual void hawaii_shell_window_mapped(struct ::wl_hawaii_shell *object,
                                            struct ::wl_hawaii_window *id,
                                            const QString &title,
                                            const QString &identifier,
                                            int32_t state);
    virtual void hawaii_shell_window_switching_started();
    virtual void hawaii_shell_window_switching_finished();
    virtual void hawaii_shell_window_switched(struct ::wl_hawaii_window *window);
    virtual void hawaii_shell_workspace_added(struct ::wl_hawaii_shell *object,
                                              struct ::wl_hawaii_workspace *workspace,
                                              int32_t active);

private:
    HawaiiShellPrivate *m_parent;
};

class HawaiiShellPrivate
{
    Q_DECLARE_PUBLIC(HawaiiShell)
public:
    HawaiiShellPrivate(HawaiiShell *parent);
    ~HawaiiShellPrivate();

    HawaiiShell *q_ptr;

    wl_display *display;
    wl_registry *registry;
    HawaiiShellImpl *shell;
    QtWayland::wl_notification_daemon *notifications;

    QElapsedTimer elapsedTimer;
    QQmlEngine *engine;
    QHash<QString, QObject *> services;
    QQmlComponent *registrar;
    ShellUi *uiController;
    QList<Window *> windows;
    QList<Workspace *> workspaces;
    QList<KeyBinding *> keyBindings;

    bool windowsMinimized;

    void emitWindowAdded(Window *window);
    void emitWindowSwitching(bool started);
    void emitWindowSwitchingNext(Window *window);
    void emitWorkspaceAdded(int num);

    static void handleGlobal(void *data, wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version);
    static void handleGlobalRemove(void *data,
                                   wl_registry *registry,
                                   uint32_t name);

    static const struct wl_registry_listener registryListener;
};

#endif // HAWAIISHELL_P_H
