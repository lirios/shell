/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef DESKTOPSHELL_P_H
#define DESKTOPSHELL_P_H

#include <QtCore/QElapsedTimer>

#include "desktopshell.h"
#include "qwayland-desktop.h"
#include "qwayland-notification.h"

class DesktopShellImpl : public QtWayland::hawaii_desktop_shell
{
public:
    DesktopShellImpl(DesktopShellPrivate *parent);

protected:
    virtual void hawaii_desktop_shell_loaded();
    virtual void hawaii_desktop_shell_prepare_lock_surface();
    virtual void hawaii_desktop_shell_grab_cursor(uint32_t cursor);
    virtual void hawaii_desktop_shell_window_mapped(struct ::hawaii_desktop_shell *object,
                                                    struct ::hawaii_window *id,
                                                    const QString &title,
                                                    const QString &identifier,
                                                    int32_t state);
    virtual void hawaii_desktop_shell_workspace_added(struct ::hawaii_desktop_shell *object,
                                                      struct ::hawaii_workspace *workspace,
                                                      int32_t active);

private:
    DesktopShellPrivate *m_parent;
};

class DesktopShellPrivate
{
    Q_DECLARE_PUBLIC(DesktopShell)
public:
    DesktopShellPrivate(DesktopShell *parent);
    ~DesktopShellPrivate();

    DesktopShell *q_ptr;

    wl_display *display;
    wl_registry *registry;
    DesktopShellImpl *shell;
    QtWayland::wl_notification_daemon *notifications;

    QElapsedTimer elapsedTimer;
    QQmlEngine *engine;
    QHash<QString, QObject *> services;
    QList<ShellUi *> shellWindows;
    QList<Window *> windows;
    QList<Workspace *> workspaces;
    QList<KeyBinding *> keyBindings;

    bool windowsMinimized;

    void emitWindowAdded(Window *window);
    void emitWorkspaceAdded(int num);

    static void handleGlobal(void *data, wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version);

    static const struct wl_registry_listener registryListener;
};

#endif // DESKTOPSHELL_P_H
