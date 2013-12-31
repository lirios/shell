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

#include <QtCompositor/QWaylandSurfaceItem>

#include <private/qwlsurface_p.h>

#include "screensaver.h"
#include "shell.h"
#include "popupgrabber.h"
#include "shellsurface.h"

Shell::Shell(struct ::wl_display *display)
    : QObject()
    , QtWaylandServer::wl_hawaii_shell(display)
    , m_lockSurface(nullptr)
    , m_locked(false)
    , m_prepareEventSent(false)
{
}

Shell::~Shell()
{
    qDeleteAll(m_keyBindings);
}

bool Shell::isLocked() const
{
    return m_locked;
}

QWaylandSurface *Shell::surfaceAt(const QPointF &point, QPointF *local)
{
    QWaylandSurface *surface;

#define SURFACE_AT(layer) \
    surface = surfaceAt(layer, point, local); \
    if (surface) \
    return surface;

    SURFACE_AT(m_dialogsLayer);
    //SURFACE_AT(m_overlayLayer);
    SURFACE_AT(m_panelsLayer);
    //SURFACE_AT(m_backgroundLayer);

#undef SURFACE_AT

    return 0;
}

KeyBindings Shell::keyBindings() const
{
    return m_keyBindings;
}

void Shell::lockSession()
{
    // If session is already locked, set DPMS off
    if (m_locked) {
        Compositor::instance()->setState(Compositor::CompositorSleeping);
        return;
    }

    // Session is now locked
    m_locked = true;
    Q_EMIT lockedChanged(true);

    // Hide all surfaces
    for (QWaylandSurface *surface: m_panelsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(false);
    }
    for (QWaylandSurface *surface: m_overlayLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(false);
    }
    for (QWaylandSurface *surface: m_dialogsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(false);
    }

    // Run screensaver
    Compositor::instance()->screenSaver()->launchProcess();
}

void Shell::unlockSession()
{
    // If unlocked or the lock surface is already mapped we
    // just return, the QML side will hide the splash screen
    if (!m_locked || m_lockSurface) {
        Q_EMIT Compositor::instance()->fadeIn();
        return;
    }

    // If the shell client has gone away, unlock immediately
    if (!Compositor::instance()->isShellClientRunning()) {
        resumeDesktop();
        return;
    }

    // Just return if we already asked the shell client to
    // prepare the lock surface
    if (m_prepareEventSent)
        return;

    // Ask the shell client to prepare the lock surface
    for (Resource *resource: resourceMap())
        send_prepare_lock_surface(resource->handle);
    m_prepareEventSent = true;
}

void Shell::addSurfaceToLayer(Compositor::ShellWindowRole role, QWaylandSurface *surface)
{
    if (!surface)
        return;

    surface->setWindowProperty(QStringLiteral("role"), role);

    switch (role) {
    case Compositor::BackgroundWindowRole:
        m_backgroundLayer.append(surface);
        break;
    case Compositor::PanelWindowRole:
    case Compositor::SpecialWindowRole:
    case Compositor::PopupWindowRole:
        m_panelsLayer.append(surface);
        break;
    case Compositor::OverlayWindowRole:
        m_overlayLayer.append(surface);
        break;
    case Compositor::DialogWindowRole:
        m_dialogsLayer.append(surface);
        break;
    default:
        break;
    }

    connect(surface, &QWaylandSurface::unmapped, [=]() {
        removeSurfaceFromLayer(surface);
    });
}

void Shell::removeSurfaceFromLayer(QWaylandSurface *surface)
{
    if (!surface)
        return;

    bool found = false;
    Compositor::ShellWindowRole role = static_cast<Compositor::ShellWindowRole>(
                surface->windowProperties().value(QStringLiteral("role")).toInt(&found));
    if (!found)
        return;

    switch (role) {
    case Compositor::BackgroundWindowRole:
        m_backgroundLayer.removeOne(surface);
        break;
    case Compositor::PanelWindowRole:
    case Compositor::SpecialWindowRole:
    case Compositor::PopupWindowRole:
        m_panelsLayer.removeOne(surface);
        break;
    case Compositor::OverlayWindowRole:
        m_overlayLayer.removeOne(surface);
        break;
    case Compositor::DialogWindowRole:
        m_dialogsLayer.removeOne(surface);
        break;
    default:
        break;
    }
}

QWaylandSurface *Shell::surfaceAt(const Layer &layer, const QPointF &point, QPointF *local)
{
    // Iterate through the layer in reverse order
    for (int i = layer.size() - 1; i >= 0; --i) {
        QWaylandSurface *surface = layer.at(i);

        // Ignore hidden surfaces or surfaces not mapped
        if (!surface->visible() || !surface->surfaceItem())
            continue;

        // Are the coordinates inside an item?
        QWaylandSurfaceItem *item = surface->surfaceItem();
        QRectF geo(item->position(), QSizeF(item->width(), item->height()));
        if (geo.contains(point)) {
            if (local)
                *local = point - item->position();
            return surface;
        }
    }

    return 0;
}

void Shell::resumeDesktop()
{
    // Terminate screen saver process
    Compositor::instance()->screenSaver()->terminateProcess();

    // Show all surfaces and hide the lock surface
    for (QWaylandSurface *surface: m_panelsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(true);
    }
    for (QWaylandSurface *surface: m_overlayLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(true);
    }
    for (QWaylandSurface *surface: m_dialogsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(true);
    }
    if (m_lockSurface && m_lockSurface->surfaceItem())
        m_lockSurface->surfaceItem()->setVisible(false);

    // Unlock the session and reveal the desktop
    m_locked = false;
    Q_EMIT lockedChanged(false);
}

void Shell::hawaii_shell_bind_resource(Resource *resource)
{
    send_loaded(resource->handle);
}

void Shell::hawaii_shell_add_key_binding(Resource *resource, uint32_t id,
                                         uint32_t key, uint32_t modifiers)
{
    Q_UNUSED(resource);

    KeyBinding *keyBinding = new KeyBinding(resource->client(), id);
    keyBinding->setModifiers(modifiers);
    keyBinding->setKey(key);
    m_keyBindings.append(keyBinding);
}

void Shell::hawaii_shell_set_background(Resource *resource,
                                        struct ::wl_resource *output_resource,
                                        struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::BackgroundWindowRole, surface);
}

void Shell::hawaii_shell_set_panel(Resource *resource,
                                   struct ::wl_resource *output_resource,
                                   struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::PanelWindowRole, surface);
}

void Shell::hawaii_shell_set_special(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::SpecialWindowRole, surface);
}

void Shell::hawaii_shell_set_overlay(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::OverlayWindowRole, surface);
}

void Shell::hawaii_shell_set_popup(Resource *resource,
                                   uint32_t id,
                                   struct ::wl_resource *output_resource,
                                   struct ::wl_resource *surface_resource,
                                   int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    if (m_panelsLayer.contains(surface))
        return;
    surface->setWindowProperty(QStringLiteral("position"), QPointF(x, y));
    addSurfaceToLayer(Compositor::PopupWindowRole, surface);

    QtWayland::InputDevice *input = Compositor::instance()->defaultInputDevice()->handle();

    ShellSurface *shellSurface = new ShellSurface(Compositor::PopupWindowRole, surface);
    shellSurface->init(resource->client(), id);

    PopupGrabber *grabber = new PopupGrabber(input);
    wl_resource_set_user_data(shellSurface->resource()->handle, grabber);
    shellSurface->setPopupGrabber(grabber);
}

void Shell::hawaii_shell_set_dialog(Resource *resource,
                                    struct ::wl_resource *output_resource,
                                    struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::DialogWindowRole, surface);
}

void Shell::hawaii_shell_set_position(Resource *resource,
                                      struct ::wl_resource *surface_resource,
                                      int32_t x, int32_t y)
{
    Q_UNUSED(resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), QPointF(x, y));
    surface->setPos(QPointF(x, y));
}

void Shell::hawaii_shell_set_lock_surface(Resource *resource,
                                          struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);

    m_prepareEventSent = false;

    if (!m_locked)
        return;

    m_lockSurface = QtWayland::Surface::fromResource(
                surface_resource)->waylandSurface();
    connect(m_lockSurface, &QWaylandSurface::mapped, [=]() {
#ifdef QT_COMPOSITOR_QUICK
        if (m_lockSurface->surfaceItem()) {
            m_lockSurface->surfaceItem()->setZ(999);
            m_lockSurface->surfaceItem()->takeFocus();
        }
#endif

        Q_EMIT Compositor::instance()->fadeIn();
    });
    connect(m_lockSurface, &QObject::destroyed, [=](QObject *object = 0) {
        m_lockSurface = nullptr;
    });
}

void Shell::hawaii_shell_quit(Resource *resource)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_lock(Resource *resource)
{
    Q_UNUSED(resource);

    // Don't lock again in case we're already locked
    if (m_locked || m_lockSurface)
        return;

    Compositor::instance()->setState(Compositor::CompositorIdle);
}

void Shell::hawaii_shell_unlock(Resource *resource)
{
    Q_UNUSED(resource);

    // Reset the flag for the prepare lock surface event
    m_prepareEventSent = false;

    // Resume the desktop if we are unlocked
    if (m_locked)
        resumeDesktop();
}

void Shell::hawaii_shell_set_grab_surface(Resource *resource,
                                          struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_desktop_ready(Resource *resource)
{
    Q_UNUSED(resource);
    Q_EMIT ready();
}

void Shell::hawaii_shell_minimize_windows(Resource *resource)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_restore_windows(Resource *resource)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_add_workspace(Resource *resource)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_select_workspace(Resource *resource,
                                          struct ::wl_resource *workspace)
{
    Q_UNUSED(resource);
}

void Shell::hawaii_shell_start_grab(Resource *resource, uint32_t id)
{
    Q_UNUSED(resource);
}

#include "moc_shell.cpp"
