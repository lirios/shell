/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_GTKSHELL_P_H
#define LIRI_GTKSHELL_P_H

#include "gtkshell.h"
#include "qwayland-server-gtk-shell.h"

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

class GtkShellPrivate : public QtWaylandServer::gtk_shell
{
    Q_DECLARE_PUBLIC(GtkShell)
public:
    GtkShellPrivate(GtkShell *self);

    static GtkShellPrivate *get(GtkShell *shell) { return shell->d_func(); }

protected:
    GtkShell *q_ptr;

    void gtk_shell_bind_resource(Resource *resource) override;

    void gtk_shell_get_gtk_surface(Resource *resource, uint32_t id,
                                   wl_resource *surfaceResource)  override;
};

class GtkSurfacePrivate : public QtWaylandServer::gtk_surface
{
    Q_DECLARE_PUBLIC(GtkSurface)
public:
    GtkSurfacePrivate(GtkSurface *self);

    static GtkSurfacePrivate *get(GtkSurface *surface) { return surface->d_func(); }

protected:
    void gtk_surface_destroy_resource(Resource *resource) override;

    void gtk_surface_set_dbus_properties(Resource *resource,
                                     const QString &application_id,
                                     const QString &app_menu_path,
                                     const QString &menubar_path,
                                     const QString &window_object_path,
                                     const QString &application_object_path,
                                     const QString &unique_bus_name) override;

    void gtk_surface_set_modal(Resource *resource) override;
    void gtk_surface_unset_modal(Resource *resource) override;

protected:
    GtkSurface *q_ptr;

private:
    GtkShell *m_shell;
    QWaylandSurface *m_surface;

    QString m_appId;
    QString m_appMenuPath;
    QString m_menuBarPath;
    QString m_windowObjectPath;
    QString m_appObjectPath;
    QString m_uniqueBusName;
};

#endif // LIRI_GTKSHELL_P_H

