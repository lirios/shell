/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELL_H
#define SHELL_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCompositor/QWaylandSurface>

#include "qwayland-server-hawaii.h"
#include "compositor.h"
#include "keybinding.h"

typedef QList<QWaylandSurface *> Layer;

class Shell : public QObject,
        public QtWaylandServer::wl_hawaii_shell
{
    Q_OBJECT
    Q_PROPERTY(bool locked READ isLocked NOTIFY lockedChanged)
public:
    Shell(struct ::wl_display *display);
    ~Shell();

    bool isLocked() const;

    QWaylandSurface *surfaceAt(const QPointF &point, QPointF *local);

    KeyBindings keyBindings() const;

    void lockSession();
    void unlockSession();

Q_SIGNALS:
    void ready();

    void lockedChanged(bool value);

private:
    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_overlayLayer;
    Layer m_dialogsLayer;

    KeyBindings m_keyBindings;

    QWaylandSurface *m_lockSurface;
    bool m_locked;
    bool m_prepareEventSent;

    void addSurfaceToLayer(Compositor::ShellWindowRole role, QWaylandSurface *surface);
    void removeSurfaceFromLayer(QWaylandSurface *surface);

    QWaylandSurface *surfaceAt(const Layer &layer, const QPointF &point, QPointF *local);

    void resumeDesktop();

    void hawaii_shell_bind_resource(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_add_key_binding(Resource *resource, uint32_t id,
                                      uint32_t key, uint32_t modifiers) Q_DECL_OVERRIDE;

    void hawaii_shell_set_background(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_panel(Resource *resource,
                                struct ::wl_resource *output_resource,
                                struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_special(Resource *resource,
                                  struct ::wl_resource *output_resource,
                                  struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_overlay(Resource *resource,
                                  struct ::wl_resource *output_resource,
                                  struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_popup(Resource *resource, uint32_t id,
                                struct ::wl_resource *output_resource,
                                struct ::wl_resource *surface,
                                int32_t x, int32_t y) Q_DECL_OVERRIDE;
    void hawaii_shell_set_dialog(Resource *resource,
                                 struct ::wl_resource *output_resource,
                                 struct ::wl_resource *surface) Q_DECL_OVERRIDE;

    void hawaii_shell_set_position(Resource *resource,
                                   struct ::wl_resource *surface,
                                   int32_t x, int32_t y) Q_DECL_OVERRIDE;

    void hawaii_shell_set_lock_surface(Resource *resource,
                                       struct ::wl_resource *surface_resource) Q_DECL_OVERRIDE;

    void hawaii_shell_quit(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_lock(Resource *resource) Q_DECL_OVERRIDE;
    void hawaii_shell_unlock(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_set_grab_surface(Resource *resource,
                                       struct ::wl_resource *surface_resource) Q_DECL_OVERRIDE;

    void hawaii_shell_desktop_ready(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_minimize_windows(Resource *resource) Q_DECL_OVERRIDE;
    void hawaii_shell_restore_windows(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_add_workspace(Resource *resource) Q_DECL_OVERRIDE;
    void hawaii_shell_select_workspace(Resource *resource,
                                       struct ::wl_resource *workspace) Q_DECL_OVERRIDE;

    void hawaii_shell_start_grab(Resource *resource, uint32_t id) Q_DECL_OVERRIDE;
};

#endif // SHELL_H
