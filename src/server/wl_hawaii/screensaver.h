/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include "interface.h"
#include "wayland-screensaver-server-protocol.h"

#include <wayland-server.h>
#include <weston/compositor.h>

#include <string>

class ScreenSaver : public Interface
{
public:
    ScreenSaver();

    void launchProcess();
    void terminateProcess();

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);
    void unbind(wl_resource *resource);

    void sigChild(int status);

    int timeout();

    void configure(weston_surface *es);
    void setSurface(wl_client *client,
                    wl_resource *resource,
                    wl_resource *output_resource,
                    wl_resource *surface_resource);

    wl_resource *m_binding;

    wl_event_source *m_timer;
    bool m_enabled;
    std::string m_path;
    int m_duration;

    struct ScreenSaverChild {
        ScreenSaver *screenSaver;
        struct weston_process process;
        struct wl_client *client;
    };
    ScreenSaverChild m_child;

    static const struct wl_screensaver_interface implementation;
};

#endif // SCREENSAVER_H
