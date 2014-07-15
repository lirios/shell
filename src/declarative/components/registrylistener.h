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

#ifndef REGISTRYLISTENER_H
#define REGISTRYLISTENER_H

#include "qwayland-hawaii.h"

class RegistryListener
{
public:
    RegistryListener();
    ~RegistryListener();

    static RegistryListener *instance();

    QtWayland::hawaii_shell_surface *shellSurface() const;

private:
    QtWayland::hawaii_shell_surface *m_shellSurface;

    static void handleGlobal(void *data, struct ::wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version);
    static void handleGlobalRemove(void *data,
                                   struct ::wl_registry *registry,
                                   uint32_t name);

    static const struct wl_registry_listener listener;
};

#endif // REGISTRYLISTENER_H
