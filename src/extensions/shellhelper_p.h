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

#pragma once

#include "qwayland-server-shell-helper.h"

QT_FORWARD_DECLARE_CLASS(QWaylandSurface)

class ProcessRunner;
class ShellHelper;

class ShellHelperPrivate : public QtWaylandServer::liri_shell
{
    Q_DECLARE_PUBLIC(ShellHelper)
public:
    explicit ShellHelperPrivate(ShellHelper *qq);
    ~ShellHelperPrivate();

    static ShellHelperPrivate *get(ShellHelper *shell);

    ProcessRunner *processRunner = nullptr;
    QWaylandSurface *grabSurface = nullptr;

protected:
    ShellHelper *q_ptr;

private:
    void liri_shell_bind_resource(Resource *r) override;
    void liri_shell_set_grab_surface(Resource *resource, struct ::wl_resource *wlSurface) override;
};
