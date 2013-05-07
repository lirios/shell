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

#include <stdio.h>

#include <weston/compositor.h>

#include "shellsurface.h"
#include "shell.h"
#include "shellseat.h"

#include "wayland-desktop-shell-server-protocol.h"

ShellSurface::ShellSurface(Shell *shell, struct weston_surface *surface)
            : m_shell(shell)
            , m_workspace(nullptr)
            , m_surface(surface)
            , m_type(Type::None)
            , m_pendingType(Type::None)
            , m_unresponsive(false)
            , m_parent(nullptr)
            , m_pingTimer(nullptr)
{
    m_popup.seat = nullptr;
    wl_list_init(&m_fullscreen.transform.link);
    m_fullscreen.blackSurface = nullptr;

    m_surfaceDestroyListener.listen(&surface->surface.resource.destroy_signal);
    m_surfaceDestroyListener.signal->connect(this, &ShellSurface::surfaceDestroyed);
}

ShellSurface::~ShellSurface()
{
    if (m_popup.seat) {
        m_popup.seat->removePopupGrab(this);
    }
    destroyPingTimer();
    m_shell->removeShellSurface(this);
}

void ShellSurface::init(uint32_t id, Workspace *workspace)
{
    m_resource.destroy = [](struct wl_resource *resource) { delete static_cast<ShellSurface *>(resource->data); };
    m_resource.object.id = id;
    m_resource.object.interface = &wl_shell_surface_interface;
    m_resource.object.implementation = &m_shell_surface_implementation;
    m_resource.data = this;

    m_workspace = workspace;
}

void ShellSurface::surfaceDestroyed()
{
    if (m_resource.client) {
        wl_resource_destroy(&m_resource);
    } else {
        delete this;
    }
}

bool ShellSurface::updateType()
{
    if (m_type != m_pendingType && m_pendingType != Type::None) {
        switch (m_type) {
            case Type::Maximized:
                unsetMaximized();
                break;
            case Type::Fullscreen:
                unsetFullscreen();
                break;
            default:
                break;
        }
        m_type = m_pendingType;
        m_pendingType = Type::None;

        switch (m_type) {
            case Type::Maximized:
            case Type::Fullscreen:
                m_savedX = x();
                m_savedY = y();
                break;
            case Type::Transient:
                weston_surface_set_position(m_surface, m_parent->geometry.x + m_transient.x, m_parent->geometry.y + m_transient.y);
                break;
            default:
                break;
        }
        return true;
    }
    return false;
}

void ShellSurface::map(int32_t x, int32_t y, int32_t width, int32_t height)
{
    m_surface->geometry.width = width;
    m_surface->geometry.height = height;
    weston_surface_geometry_dirty(m_surface);

    switch (m_type) {
        case Type::Popup:
            mapPopup();
            break;
        case Type::Fullscreen:
            centerOnOutput(m_fullscreen.output);
            break;
        case Type::Maximized: {
            IRect2D rect = m_shell->windowsArea(m_output);
            x = rect.x;
            y = rect.y;
        }
        case Type::TopLevel:
        case Type::None:
            weston_surface_set_position(m_surface, x, y);
        default:
            break;
    }

    printf("map %d %d %d %d - %d\n",x,y,width,height,m_type);


    if (m_type != Type::None) {
        weston_surface_update_transform(m_surface);
        if (m_type == Type::Maximized) {
            m_surface->output = m_output;
        }
    }
}

void ShellSurface::unmapped()
{
    if (m_popup.seat) {
        m_popup.seat->removePopupGrab(this);
        m_popup.seat = nullptr;
    }
}

void ShellSurface::mapPopup()
{
    m_surface->output = m_parent->output;

    weston_surface_set_transform_parent(m_surface, m_parent);
    weston_surface_set_position(m_surface, m_popup.x, m_popup.y);
    weston_surface_update_transform(m_surface);

    if (!m_popup.seat->addPopupGrab(this, m_popup.serial)) {
        popupDone();
    }
}

void ShellSurface::addTransform(struct weston_transform *transform)
{
    removeTransform(transform);
    wl_list_insert(&m_surface->geometry.transformation_list, &transform->link);

    damage();
}

void ShellSurface::removeTransform(struct weston_transform *transform)
{
    if (wl_list_empty(&transform->link)) {
        return;
    }

    wl_list_remove(&transform->link);
    wl_list_init(&transform->link);

    damage();
}

void ShellSurface::damage()
{
    weston_surface_geometry_dirty(m_surface);
    weston_surface_damage(m_surface);
}

void ShellSurface::setAlpha(float alpha)
{
    m_surface->alpha = alpha;
    damage();
}

void ShellSurface::popupDone()
{
    wl_shell_surface_send_popup_done(&m_resource);
    m_popup.seat = nullptr;
}

bool ShellSurface::isMapped() const
{
    return weston_surface_is_mapped(m_surface);
}

int32_t ShellSurface::x() const
{
    return m_surface->geometry.x;
}

int32_t ShellSurface::y() const
{
    return m_surface->geometry.y;
}

int32_t ShellSurface::width() const
{
    return m_surface->geometry.width;
}

int32_t ShellSurface::height() const
{
    return m_surface->geometry.height;
}

int32_t ShellSurface::transformedWidth() const
{
    pixman_box32_t *box = pixman_region32_extents(&m_surface->transform.boundingbox);
    return box->x2 - box->x1;
}

int32_t ShellSurface::transformedHeight() const
{
    pixman_box32_t *box = pixman_region32_extents(&m_surface->transform.boundingbox);
    return box->y2 - box->y1;
}

float ShellSurface::alpha() const
{
    return m_surface->alpha;
}

bool ShellSurface::isPopup() const
{
    return m_popup.seat != nullptr;
}

ShellSurface *ShellSurface::topLevelParent()
{
    if (isPopup() && m_parent) {
        ShellSurface *p = Shell::getShellSurface(m_parent);
        if (p) {
            return p->topLevelParent();
        }
        return nullptr;
    }

    return this;
}

struct weston_surface *ShellSurface::transformParent() const
{
    return m_surface->geometry.parent;
}

void ShellSurface::setFullscreen(uint32_t method, uint32_t framerate, struct weston_output *output)
{
    if (output) {
        m_output = output;
    } else if (m_surface->output) {
        m_output = m_surface->output;
    } else {
        m_output = m_shell->getDefaultOutput();
    }

    m_fullscreen.output = m_output;
    m_fullscreen.type = (enum wl_shell_surface_fullscreen_method)method;
    m_fullscreen.framerate = framerate;
    m_pendingType = Type::Fullscreen;

    m_client->send_configure(m_surface, 0, m_output->width, m_output->height);
}

void ShellSurface::unsetFullscreen()
{
    m_fullscreen.type = WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT;
    m_fullscreen.framerate = 0;
    removeTransform(&m_fullscreen.transform);
    if (m_fullscreen.blackSurface) {
        weston_surface_destroy(m_fullscreen.blackSurface);
    }
    m_fullscreen.blackSurface = nullptr;
    m_fullscreen.output = nullptr;
    weston_surface_set_position(m_surface, m_savedX, m_savedY);
}

void ShellSurface::unsetMaximized()
{
    weston_surface_set_position(m_surface, m_savedX, m_savedY);
}

void ShellSurface::centerOnOutput(struct weston_output *output)
{
    int32_t width = weston_surface_buffer_width(m_surface);
    int32_t height = weston_surface_buffer_height(m_surface);
    float x, y;

    x = output->x + (output->width - width) / 2;
    y = output->y + (output->height - height) / 2;

    weston_surface_configure(m_surface, x, y, width, height);
}


int ShellSurface::pingTimeout()
{
    m_unresponsive = true;
    pingTimeoutSignal(this);

    return 1;
}

void ShellSurface::ping(uint32_t serial)
{
    const int ping_timeout = 200;

    if (!m_resource.client)
        return;

    if (!m_pingTimer) {
        m_pingTimer = new PingTimer;
        if (!m_pingTimer)
            return;

        m_pingTimer->serial = serial;
        struct wl_event_loop *loop = wl_display_get_event_loop(m_surface->compositor->wl_display);
        m_pingTimer->source = wl_event_loop_add_timer(loop, [](void *data)
                                                     { return static_cast<ShellSurface *>(data)->pingTimeout(); }, this);
        wl_event_source_timer_update(m_pingTimer->source, ping_timeout);

        wl_shell_surface_send_ping(&m_resource, serial);
    }
}

bool ShellSurface::isResponsive() const
{
    return !m_unresponsive;
}

void ShellSurface::pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
    if (!m_pingTimer)
        /* Just ignore unsolicited pong. */
        return;

    if (m_pingTimer->serial == serial) {
        pongSignal(this);
        m_unresponsive = false;
        destroyPingTimer();
    }
}

void ShellSurface::destroyPingTimer()
{
    if (m_pingTimer && m_pingTimer->source)
        wl_event_source_remove(m_pingTimer->source);

    delete m_pingTimer;
    m_pingTimer = nullptr;
}

// -- Move --

struct MoveGrab : public ShellGrab {
    struct ShellSurface *shsurf;
    struct wl_listener shsurf_destroy_listener;
    wl_fixed_t dx, dy;
};

void ShellSurface::move_grab_motion(struct wl_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
    ShellGrab *shgrab = container_of(grab, ShellGrab, grab);
    MoveGrab *move = static_cast<MoveGrab *>(shgrab);
    struct wl_pointer *pointer = grab->pointer;
    ShellSurface *shsurf = move->shsurf;
    int dx = wl_fixed_to_int(pointer->x + move->dx);
    int dy = wl_fixed_to_int(pointer->y + move->dy);

    if (!shsurf)
        return;

    struct weston_surface *es = shsurf->m_surface;

    weston_surface_configure(es, dx, dy, es->geometry.width, es->geometry.height);

    weston_compositor_schedule_repaint(es->compositor);
}

void ShellSurface::move_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w)
{
    ShellGrab *shell_grab = container_of(grab, ShellGrab, grab);
    MoveGrab *move = static_cast<MoveGrab *>(shell_grab);
    struct wl_pointer *pointer = grab->pointer;
    enum wl_pointer_button_state state = (wl_pointer_button_state)state_w;

    if (pointer->button_count == 0 && state == WL_POINTER_BUTTON_STATE_RELEASED) {
        Shell::endGrab(shell_grab);
        move->shsurf->moveEndSignal(move->shsurf);
        delete shell_grab;
    }
}

const struct wl_pointer_grab_interface ShellSurface::m_move_grab_interface = {
    [](struct wl_pointer_grab *grab, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y) {},
    ShellSurface::move_grab_motion,
    ShellSurface::move_grab_button,
};

void ShellSurface::move(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                        uint32_t serial)
{
    struct weston_seat *ws = static_cast<weston_seat *>(seat_resource->data);

    if (ws->seat.pointer->button_count == 0 || ws->seat.pointer->grab_serial != serial ||
        ws->seat.pointer->focus != &m_surface->surface) {
        return;
    }

//     if (shsurf->type == SHELL_SURFACE_FULLSCREEN)
//         return 0;
    dragMove(ws);
}

void ShellSurface::dragMove(struct weston_seat *ws)
{
    MoveGrab *move = new MoveGrab;
    if (!move)
        return;

    move->dx = wl_fixed_from_double(m_surface->geometry.x) - ws->seat.pointer->grab_x;
    move->dy = wl_fixed_from_double(m_surface->geometry.y) - ws->seat.pointer->grab_y;
    move->shsurf = this;
    move->grab.focus = &m_surface->surface;

    m_shell->startGrab(move, &m_move_grab_interface, ws, DESKTOP_SHELL_CURSOR_MOVE);
    moveStartSignal(this);
}

// -- Resize --

struct ResizeGrab : public ShellGrab {
    struct ShellSurface *shsurf;
    struct wl_listener shsurf_destroy_listener;
    uint32_t edges;
    int32_t width, height;
};

void ShellSurface::resize_grab_motion(struct wl_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
    ShellGrab *shgrab = container_of(grab, ShellGrab, grab);
    ResizeGrab *resize = static_cast<ResizeGrab *>(shgrab);
    struct wl_pointer *pointer = grab->pointer;
    ShellSurface *shsurf = resize->shsurf;

    if (!shsurf)
        return;

    struct weston_surface *es = shsurf->m_surface;

    wl_fixed_t from_x, from_y;
    wl_fixed_t to_x, to_y;
    weston_surface_from_global_fixed(es, pointer->grab_x, pointer->grab_y, &from_x, &from_y);
    weston_surface_from_global_fixed(es, pointer->x, pointer->y, &to_x, &to_y);

    int32_t width = resize->width;
    if (resize->edges & WL_SHELL_SURFACE_RESIZE_LEFT) {
        width += wl_fixed_to_int(from_x - to_x);
    } else if (resize->edges & WL_SHELL_SURFACE_RESIZE_RIGHT) {
        width += wl_fixed_to_int(to_x - from_x);
    }

    int32_t height = resize->height;
    if (resize->edges & WL_SHELL_SURFACE_RESIZE_TOP) {
        height += wl_fixed_to_int(from_y - to_y);
    } else if (resize->edges & WL_SHELL_SURFACE_RESIZE_BOTTOM) {
        height += wl_fixed_to_int(to_y - from_y);
    }

    shsurf->m_client->send_configure(shsurf->m_surface, resize->edges, width, height);
}

void ShellSurface::resize_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w)
{
    ShellGrab *shgrab = container_of(grab, ShellGrab, grab);
    ResizeGrab *resize = static_cast<ResizeGrab *>(shgrab);

    if (grab->pointer->button_count == 0 && state_w == WL_POINTER_BUTTON_STATE_RELEASED) {
        Shell::endGrab(resize);
        delete resize;
    }
}

const struct wl_pointer_grab_interface ShellSurface::m_resize_grab_interface = {
    [](struct wl_pointer_grab *grab, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y) {},
    ShellSurface::resize_grab_motion,
    ShellSurface::resize_grab_button,
};

void ShellSurface::resize(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                          uint32_t serial, uint32_t edges)
{
    struct weston_seat *ws = static_cast<weston_seat *>(seat_resource->data);

    if (ws->seat.pointer->button_count == 0 || ws->seat.pointer->grab_serial != serial ||
        ws->seat.pointer->focus != &m_surface->surface) {
        return;
    }

    dragResize(ws, edges);
}

void ShellSurface::dragResize(struct weston_seat *ws, uint32_t edges)
{
    ResizeGrab *grab = new ResizeGrab;
    if (!grab)
        return;

    if (edges == 0 || edges > 15 || (edges & 3) == 3 || (edges & 12) == 12) {
        return;
    }

    grab->edges = edges;
    grab->width = m_surface->geometry.width;
    grab->height = m_surface->geometry.height;
    grab->shsurf = this;
    grab->grab.focus = &m_surface->surface;

    m_shell->startGrab(grab, &m_resize_grab_interface, ws, edges);
}

void ShellSurface::setToplevel(struct wl_client *, struct wl_resource *)
{
printf("top\n");
    m_pendingType = Type::TopLevel;
}

void ShellSurface::setTransient(struct wl_client *client, struct wl_resource *resource,
                  struct wl_resource *parent_resource, int x, int y, uint32_t flags)
{
    m_parent = static_cast<struct weston_surface *>(parent_resource->data);
    m_transient.x = x;
    m_transient.y = y;
    m_transient.flags = flags;

    m_pendingType = Type::Transient;

}

void ShellSurface::setFullscreen(struct wl_client *client, struct wl_resource *resource, uint32_t method,
                   uint32_t framerate, struct wl_resource *output_resource)
{
    struct weston_output *output = output_resource ? static_cast<struct weston_output *>(output_resource->data) : nullptr;
    setFullscreen(method, framerate, output);
}

void ShellSurface::setPopup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
              uint32_t serial, struct wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags)
{
    m_parent = static_cast<struct weston_surface *>(parent_resource->data);
    m_popup.x = x;
    m_popup.y = y;
    m_popup.serial = serial;
    m_popup.seat = ShellSeat::shellSeat(static_cast<struct weston_seat *>(seat_resource->data));

    m_pendingType = Type::Popup;
}

void ShellSurface::setMaximized(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource)
{
    /* get the default output, if the client set it as NULL
    c heck whether the ouput is available */
    if (output_resource) {
        m_output = static_cast<struct weston_output *>(output_resource->data);
    } else if (m_surface->output) {
        m_output = m_surface->output;
    } else {
        m_output = m_shell->getDefaultOutput();
    }

    uint32_t edges = WL_SHELL_SURFACE_RESIZE_TOP | WL_SHELL_SURFACE_RESIZE_LEFT;

    IRect2D rect = m_shell->windowsArea(m_output);
    m_client->send_configure(m_surface, edges, rect.width, rect.height);
    m_pendingType = Type::Maximized;
}

void ShellSurface::setTitle(struct wl_client *client, struct wl_resource *resource, const char *title)
{
    m_title = title;
}

void ShellSurface::setClass(struct wl_client *client, struct wl_resource *resource, const char *className)
{
    m_class = className;
}

const struct wl_shell_surface_interface ShellSurface::m_shell_surface_implementation = {
    ShellSurface::shell_surface_pong,
    ShellSurface::shell_surface_move,
    ShellSurface::shell_surface_resize,
    ShellSurface::shell_surface_set_toplevel,
    ShellSurface::shell_surface_set_transient,
    ShellSurface::shell_surface_set_fullscreen,
    ShellSurface::shell_surface_set_popup,
    ShellSurface::shell_surface_set_maximized,
    ShellSurface::shell_surface_set_title,
    ShellSurface::shell_surface_set_class
};
