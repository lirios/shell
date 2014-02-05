/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2014 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#ifndef HAWAIICLIENTWINDOW_H
#define HAWAIICLIENTWINDOW_H

#include <wayland-server.h>

#include "interface.h"

class ShellSurface;

class HawaiiClientWIndow : public Interface
{
public:
    HawaiiClientWIndow();
    ~HawaiiClientWIndow();

    void create();

protected:
    virtual void added() override;

private:
    ShellSurface *shsurf();
    void surfaceTypeChanged();
    void activeChanged();
    void mapped();
    void destroy();
    void sendState();
    void sendTitle();
    void setState(wl_client *client, wl_resource *resource, int32_t state);
    void close(wl_client *client, wl_resource *resource);

    wl_resource *m_resource;
    int32_t m_state;

    static const struct wl_hawaii_window_interface s_implementation;
};

#endif // HAWAIICLIENTWINDOW_H
