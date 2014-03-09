/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "registrylistener.h"
#include "shellmanager.h"

RegistryListener::RegistryListener()
    : shell(new ShellClient())
    , shellSurface(new ShellSurfaceClient())
    , panelManager(new QtWayland::wl_hawaii_panel_manager())
{
}

RegistryListener::~RegistryListener()
{
    delete panelManager;
    delete shellSurface;
    delete shell;
}

void RegistryListener::run()
{
    // Platform native interface
    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    Q_ASSERT(native);

    // Get Wayland display
    struct ::wl_display *display = static_cast<struct ::wl_display *>(
                native->nativeResourceForIntegration("display"));
    Q_ASSERT(display);

    // Wayland registry
    struct ::wl_registry *registry = wl_display_get_registry(display);
    Q_ASSERT(registry);

    // Initialize interfaces
    wl_registry_add_listener(registry, &RegistryListener::listener, this);
}

void RegistryListener::handleGlobal(void *data,
                                    struct ::wl_registry *registry,
                                    uint32_t id,
                                    const char *interface,
                                    uint32_t version)
{
    Q_UNUSED(version);

    RegistryListener *self = static_cast<RegistryListener *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to RegistryListener pointer in handleGlobal";
        return;
    }

    if (strcmp(interface, "wl_hawaii_shell") == 0)
        self->shell->init(registry, id);
    else if (strcmp(interface, "wl_hawaii_shell_surface") == 0)
        self->shellSurface->init(registry, id);
    else if (strcmp(interface, "wl_hawaii_panel_manager") == 0)
        self->panelManager->init(registry, id);
}

void RegistryListener::handleGlobalRemove(void *data,
                                          struct ::wl_registry *registry,
                                          uint32_t name)
{
    Q_UNUSED(name);
    Q_UNUSED(registry);

    RegistryListener *self = static_cast<RegistryListener *>(data);
    if (!self) {
        qWarning() << "Unable to cast data to RegistryListener pointer in handleGlobalRemove";
        return;
    }

    wl_hawaii_panel_manager_destroy(self->panelManager->object());
    wl_hawaii_shell_surface_destroy(self->shellSurface->object());
    wl_hawaii_shell_destroy(self->shell->object());
}

const struct wl_registry_listener RegistryListener::listener = {
    RegistryListener::handleGlobal,
    RegistryListener::handleGlobalRemove
};
