/*
 * Copyright 2013  Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XWLSHELL_H
#define XWLSHELL_H

#include <wayland-server.h>

#include "interface.h"

struct weston_surface;
struct weston_shell_client;

class ShellSurface;

class XWlShell : public Interface
{
public:
    XWlShell();

    ShellSurface *getShellSurface(weston_surface *surface, const weston_shell_client *client);

private:
};

class XWlShellSurface : public Interface
{
public:
    XWlShellSurface();
};

#endif
