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

#include "notificationsdaemon.h"
#include "registrylistener.h"

Q_GLOBAL_STATIC(RegistryListener, s_registryListener)

RegistryListener::RegistryListener()
    : m_notifications(new QtWayland::wl_notification_daemon())
{
}

RegistryListener::~RegistryListener()
{
    delete m_notifications;
}

RegistryListener *RegistryListener::instance()
{
    return s_registryListener();
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

void RegistryListener::addSurface(struct ::wl_surface *surface)
{
    m_notifications->add_surface(surface);
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

    if (strcmp(interface, "wl_notification_daemon") == 0) {
        self->m_notifications->init(registry, id);

        // Start the notifications daemon and connect to the session bus
        NotificationsDaemon *daemon = NotificationsDaemon::instance();
        QMetaObject::invokeMethod(daemon, "connectOnDBus");
    }
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

    wl_notification_daemon_destroy(self->m_notifications->object());
}

const struct wl_registry_listener RegistryListener::listener = {
    RegistryListener::handleGlobal,
    RegistryListener::handleGlobalRemove
};
