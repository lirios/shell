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

#ifndef SHELL_H
#define SHELL_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCompositor/QWaylandSurface>

#include "qwayland-server-hawaii.h"

typedef QList<QWaylandSurface *> Layer;

class Shell : public QObject,
        public QtWaylandServer::wl_hawaii_shell
{
    Q_OBJECT
    Q_ENUMS(ShellWindowRole)
public:
    enum ShellWindowRole {
        BackgroundWindowRole = 0,
        PanelWindowRole,
        LauncherWindowRole,
        SpecialWindowRole,
        OverlayWindowRole,
        DialogWindowRole,
        PopupWindowRole
    };

    Shell(struct ::wl_display *display);

    QWaylandSurface *surfaceAt(const QPointF &point, QPointF *local);
    QWaylandSurface *surfaceAt(const Layer &layer, const QPointF &point, QPointF *local);

Q_SIGNALS:
    void ready();

private:
    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_overlayLayer;
    Layer m_dialogsLayer;

    void addSurfaceToLayer(ShellWindowRole role, QWaylandSurface *surface);

    void hawaii_shell_bind_resource(Resource *resource) Q_DECL_OVERRIDE;

    void hawaii_shell_set_background(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_panel(Resource *resource,
                                struct ::wl_resource *output_resource,
                                struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_set_launcher(Resource *resource,
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

    void hawaii_shell_desktop_ready(Resource *resource) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void surfaceUnmapped();
};

#endif // SHELL_H
