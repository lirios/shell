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

#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H

#include <wayland-server.h>

#include "interface.h"

class SettingsInterface : public Interface
{
public:
    SettingsInterface();

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);
    void unset(wl_client *client, wl_resource *resource, const char *path, const char *name);
    void setString(wl_client *client, wl_resource *resource, const char *path, const char *name, const char *value);
    void setInt(wl_client *client, wl_resource *resource, const char *path, const char *name, int32_t value);
    void setKeyBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t key, uint32_t mod);
    void setAxisBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t axis, uint32_t mod);
    void setHotSpotBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t hotspot);
    void setButtonBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t button, uint32_t mod);

    static const struct nuclear_settings_interface s_implementation;
};

#endif
