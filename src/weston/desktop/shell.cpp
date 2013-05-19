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
#include <stdlib.h>
#include <sys/time.h>
#include <linux/input.h>

#include <wayland-server.h>

#include <weston/compositor.h>
// #include <weston/config-parser.h>

#include "shell.h"
#include "shellsurface.h"
#include "shellseat.h"
#include "workspace.h"
#include "effect.h"
#include "desktop-shell.h"
#include "cmakedirs.h"

Binding::Binding(struct weston_binding *binding)
       : m_binding(binding)
{

}

Binding::~Binding()
{
    weston_binding_destroy(m_binding);
}

Shell::Shell(struct weston_compositor *ec)
            : m_compositor(ec)
            , m_blackSurface(nullptr)
            , m_grabSurface(nullptr)
{
    srandom(weston_compositor_get_time());
    m_child.shell = this;
    m_child.deathstamp = 0;
}

ShellGrab::ShellGrab()
{
    grab.focus = nullptr;
}

Shell::~Shell()
{
    printf("dtor\n");
}

static void
black_surface_configure(struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
}

void Shell::init()
{
    m_compositor->shell_interface.shell = this;
    m_compositor->shell_interface.create_shell_surface = [](void *shell, struct weston_surface *surface, const struct weston_shell_client *client) {
        return (struct shell_surface *)static_cast<Shell *>(shell)->createShellSurface(surface, client);
    };
    //     ec->shell_interface.set_toplevel = set_toplevel;
    //     ec->shell_interface.set_transient = set_transient;
    //     ec->shell_interface.set_fullscreen = set_fullscreen;
    //     ec->shell_interface.move = surface_move;
    //     ec->shell_interface.resize = surface_resize;

    struct wl_global *global = wl_display_add_global(m_compositor->wl_display, &wl_shell_interface, this,
                                                     [](struct wl_client *client, void *data, uint32_t version, uint32_t id) {
                                                         static_cast<Shell *>(data)->bind(client, version, id);
                                                     });

    if (!global)
        return;

    struct weston_seat *seat;
    wl_list_for_each(seat, &m_compositor->seat_list, link) {
        ShellSeat *shseat = ShellSeat::shellSeat(seat);
        shseat->pointerFocusSignal.connect(this, &Shell::pointerFocus);
    }

    for (int i = 0; i < 4; ++i) {
        m_workspaces.push_back(new Workspace(this, i));
    }

    m_fullscreenLayer.insert(&m_compositor->cursor_layer);
    m_panelsLayer.insert(&m_fullscreenLayer);
    m_backgroundLayer.insert(&m_panelsLayer);

    m_currentWorkspace = 0;
    activateWorkspace(nullptr);

    m_blackSurface = weston_surface_create(m_compositor);

    struct weston_output *out = container_of(m_compositor->output_list.next, struct weston_output, link);

    int x = 0, y = 0;
    int w = out->width, h = out->height;

    m_blackSurface->configure = black_surface_configure;
    m_blackSurface->configure_private = 0;
    weston_surface_configure(m_blackSurface, x, y, w, h);
    weston_surface_set_color(m_blackSurface, 0.0, 0.0, 0.0, 1);
    pixman_region32_fini(&m_blackSurface->opaque);
    pixman_region32_init_rect(&m_blackSurface->opaque, 0, 0, w, h);
    pixman_region32_fini(&m_blackSurface->input);
    pixman_region32_init_rect(&m_blackSurface->input, 0, 0, w, h);

    m_backgroundLayer.addSurface(m_blackSurface);

    struct wl_event_loop *loop = wl_display_get_event_loop(m_compositor->wl_display);
    wl_event_loop_add_idle(loop, [](void *data) { static_cast<Shell *>(data)->launchShellProcess(); }, this);

    weston_compositor_add_button_binding(compositor(), BTN_LEFT, (weston_keyboard_modifier)0,
                                         [](struct wl_seat *seat, uint32_t time, uint32_t button, void *data) {
                                             static_cast<Shell *>(data)->activateSurface(seat, time, button); }, this);

    bindKey(KEY_LEFT, MODIFIER_CTRL, [](struct wl_seat *seat, uint32_t time, uint32_t key, void *data) {
                                            static_cast<Shell *>(data)->selectPreviousWorkspace(); }, this);
    bindKey(KEY_RIGHT, MODIFIER_CTRL, [](struct wl_seat *seat, uint32_t time, uint32_t key, void *data) {
                                            static_cast<Shell *>(data)->selectNextWorkspace(); }, this);
}

void Shell::configureSurface(ShellSurface *surface, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    if (width == 0) {
        surface->unmapped();
        return;
    }

    if (surface->m_type == ShellSurface::Type::Fullscreen && surface->m_pendingType != ShellSurface::Type::Fullscreen &&
        surface->m_pendingType != ShellSurface::Type::None) {
        if (surface->m_fullscreen.type == WL_SHELL_SURFACE_FULLSCREEN_METHOD_DRIVER && surfaceIsTopFullscreen(surface)) {
            weston_output_switch_mode(surface->m_fullscreen.output, surface->m_fullscreen.output->origin);
        }
    }
    bool changedType = surface->updateType();

    if (!surface->isMapped()) {
        switch (surface->m_type) {
            case ShellSurface::Type::TopLevel:
                surface->map(10 + random() % 400, 10 + random() % 400, width, height);
                break;
            default:
                surface->map(surface->m_surface->geometry.x + sx, surface->m_surface->geometry.y + sy, width, height);
        }

        switch (surface->m_type) {
            case ShellSurface::Type::Transient:
            case ShellSurface::Type::Popup:
                surface->m_workspace->addSurface(surface);
                surface->m_workspace->stackAbove(surface->m_surface, surface->m_parent);
                break;
            case ShellSurface::Type::Fullscreen:
                stackFullscreen(surface);
                configureFullscreen(surface);
                break;
            case ShellSurface::Type::None:
                break;
            default:
                printf("add %p\n",surface);
                surface->m_workspace->addSurface(surface);
                m_surfaces.push_back(surface);
                for (Effect *e: m_effects) {
                    e->addSurface(surface);
                }
        }

        switch (surface->m_type) {
            case ShellSurface::Type::Transient:
                if (surface->m_transient.flags == WL_SHELL_SURFACE_TRANSIENT_INACTIVE) {
                    break;
                }
            case ShellSurface::Type::TopLevel:
            case ShellSurface::Type::Maximized: {
                struct weston_seat *seat;
                wl_list_for_each(seat, &m_compositor->seat_list, link) {
                    weston_surface_activate(surface->m_surface, seat);
                }
            } break;
            default:
                break;
        }
    } else if (changedType || sx != 0 || sy != 0 || surface->width() != width || surface->height() != height) {
        float from_x, from_y;
        float to_x, to_y;

        struct weston_surface *es = surface->m_surface;
        weston_surface_to_global_float(es, 0, 0, &from_x, &from_y);
        weston_surface_to_global_float(es, sx, sy, &to_x, &to_y);
        int x = surface->x() + to_x - from_x;
        int y = surface->y() + to_y - from_y;

        weston_surface_configure(es, x, y, width, height);

        switch (surface->m_type) {
            case ShellSurface::Type::Fullscreen:
                stackFullscreen(surface);
                configureFullscreen(surface);
                break;
            case ShellSurface::Type::Maximized: {
                IRect2D rect = windowsArea(surface->output());
                weston_surface_set_position(surface->m_surface, rect.x, rect.y);
            } break;
            default:
                surface->m_workspace->addSurface(surface);
                break;
        }

        if (surface->m_surface->output) {
            weston_surface_update_transform(surface->m_surface);

            if (surface->m_type == ShellSurface::Type::Maximized) {
                surface->m_surface->output = surface->m_output;
            }
        }
    }
}

struct weston_surface *Shell::createBlackSurface(ShellSurface *fs_surface, float x, float y, int w, int h)
{
    struct weston_surface *surface = weston_surface_create(m_compositor);
    if (!surface) {
        weston_log("no memory\n");
        return nullptr;
    }

    surface->configure = black_surface_configure;
    surface->configure_private = fs_surface;
    weston_surface_configure(surface, x, y, w, h);
    weston_surface_set_color(surface, 0.0, 0.0, 0.0, 1);
    pixman_region32_fini(&surface->opaque);
    pixman_region32_init_rect(&surface->opaque, 0, 0, w, h);
    pixman_region32_fini(&surface->input);
    pixman_region32_init_rect(&surface->input, 0, 0, w, h);

    return surface;
}

void Shell::configureFullscreen(ShellSurface *shsurf)
{
    struct weston_output *output = shsurf->m_fullscreen.output;
    struct weston_surface *surface = shsurf->m_surface;

    if (!shsurf->m_fullscreen.blackSurface) {
        shsurf->m_fullscreen.blackSurface = createBlackSurface(shsurf, output->x, output->y, output->width, output->height);
    }

    m_fullscreenLayer.stackBelow(shsurf->m_fullscreen.blackSurface, shsurf->m_surface);

    switch (shsurf->m_fullscreen.type) {
    case WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT:
        if (surface->buffer_ref.buffer) {
            shsurf->centerOnOutput(shsurf->m_fullscreen.output);
        } break;
    case WL_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE: {
        /* 1:1 mapping between surface and output dimensions */
        if (output->width == surface->geometry.width && output->height == surface->geometry.height) {
            weston_surface_set_position(surface, output->x, output->y);
            break;
        }

        struct weston_matrix *matrix = &shsurf->m_fullscreen.transform.matrix;
        weston_matrix_init(matrix);

        float output_aspect = (float) output->width / (float) output->height;
        float surface_aspect = (float) surface->geometry.width / (float) surface->geometry.height;
        float scale;
        if (output_aspect < surface_aspect) {
            scale = (float) output->width / (float) surface->geometry.width;
        } else {
            scale = (float) output->height / (float) surface->geometry.height;
        }

        weston_matrix_scale(matrix, scale, scale, 1);
        shsurf->addTransform(&shsurf->m_fullscreen.transform);
        float x = output->x + (output->width - surface->geometry.width * scale) / 2;
        float y = output->y + (output->height - surface->geometry.height * scale) / 2;
        weston_surface_set_position(surface, x, y);

    } break;
    case WL_SHELL_SURFACE_FULLSCREEN_METHOD_DRIVER:
        if (surfaceIsTopFullscreen(shsurf)) {
            struct weston_mode mode = {0, surface->geometry.width, surface->geometry.height, shsurf->m_fullscreen.framerate};

            if (weston_output_switch_mode(output, &mode) == 0) {
                weston_surface_configure(shsurf->m_fullscreen.blackSurface, output->x, output->y, output->width, output->height);
                weston_surface_set_position(surface, output->x, output->y);
                break;
            }
        }
        break;
    case WL_SHELL_SURFACE_FULLSCREEN_METHOD_FILL:
        break;
    default:
        break;
    }
}

void Shell::stackFullscreen(ShellSurface *shsurf)
{
    m_fullscreenLayer.addSurface(shsurf);
    weston_surface_damage(shsurf->m_surface);

    if (!shsurf->m_fullscreen.blackSurface) {
        struct weston_output *output = shsurf->m_fullscreen.output;
        shsurf->m_fullscreen.blackSurface = createBlackSurface(shsurf, output->x, output->y, output->width, output->height);
    }

    m_fullscreenLayer.stackBelow(shsurf->m_fullscreen.blackSurface, shsurf->m_surface);
    weston_surface_damage(shsurf->m_fullscreen.blackSurface);
}

bool Shell::surfaceIsTopFullscreen(ShellSurface *surface)
{
    if (m_fullscreenLayer.isEmpty()) {
        return false;
    }

    struct weston_surface *top = *m_fullscreenLayer.begin();
    return top == surface->m_surface;
}

bool Shell::isInFullscreen() const
{
    return m_fullscreenLayer.isVisible();
}

static void shell_surface_configure(struct weston_surface *surf, int32_t sx, int32_t sy, int32_t w, int32_t h)
{
    ShellSurface *shsurf = Shell::getShellSurface(surf);
    if (shsurf) {
        shsurf->shell()->configureSurface(shsurf, sx, sy, w, h);
    }
}

ShellSurface *Shell::createShellSurface(struct weston_surface *surface, const struct weston_shell_client *client)
{
    ShellSurface *shsurf = new ShellSurface(this, surface);

    surface->configure = shell_surface_configure;
    surface->configure_private = shsurf;
    shsurf->m_client = client;
    return shsurf;
}

void Shell::sendConfigure(struct weston_surface *surface, uint32_t edges, int32_t width, int32_t height)
{
    ShellSurface *shsurf = Shell::getShellSurface(surface);
    wl_shell_surface_send_configure(&shsurf->m_resource, edges, width, height);
}

const struct weston_shell_client Shell::shell_client = {
    Shell::sendConfigure
};

ShellSurface *Shell::getShellSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id,
                                     struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);

    ShellSurface *shsurf = getShellSurface(surface);
    if (shsurf) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "desktop_shell::get_shell_surface already requested");
        return shsurf;
    }

    shsurf = createShellSurface(surface, &shell_client);
    if (!shsurf) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface->configure already set");
        return nullptr;
    }

    shsurf->init(id, currentWorkspace());
    shsurf->pingTimeoutSignal.connect(this, &Shell::pingTimeout);

    wl_client_add_resource(client, shsurf->wl_resource());

    return shsurf;
}

ShellSurface *Shell::getShellSurface(struct weston_surface *surf)
{
    if (surf->configure == shell_surface_configure) {
        return static_cast<ShellSurface *>(surf->configure_private);
    }

    return nullptr;
}

void Shell::removeShellSurface(ShellSurface *surface)
{
    for (Effect *e: m_effects) {
        e->removeSurface(surface);
    }
}

Binding *Shell::bindKey(uint32_t key, enum weston_keyboard_modifier modifier, weston_key_binding_handler_t handler, void *data)
{
    return new Binding(weston_compositor_add_key_binding(compositor(), key, modifier, handler, data));
}

void Shell::registerEffect(Effect *effect)
{
    m_effects.push_back(effect);
    for (ShellSurface *s: m_surfaces) {
        effect->addSurface(s);
    }
}

void Shell::activateSurface(ShellSurface *shsurf, struct weston_seat *seat)
{
    weston_surface_activate(shsurf->m_surface, seat);
    shsurf->m_workspace->restack(shsurf);
}

void Shell::activateSurface(struct wl_seat *seat, uint32_t time, uint32_t button)
{
    struct weston_surface *focus = (struct weston_surface *) seat->pointer->focus;
    if (!focus)
        return;

//     struct weston_surface *upper;
//     if (is_black_surface(focus, &upper))
//         focus = upper;

//     if (get_shell_surface_type(focus) == SHELL_SURFACE_NONE)
//         return;

    if (seat->pointer->grab == &seat->pointer->default_grab) {
        ShellSurface *shsurf = getShellSurface(focus);
        if (shsurf) {
            activateSurface(shsurf, (struct weston_seat *)seat);
        }
//         activate(shell, focus, (struct weston_seat *)seat)
    };
}

void Shell::startGrab(ShellGrab *grab, const struct wl_pointer_grab_interface *interface,
                      struct weston_seat *seat, uint32_t cursor)
{
    ShellSeat::shellSeat(seat)->endPopupGrab();

    grab->shell = this;
    grab->grab.interface = interface;
//     grab->shsurf = shsurf;
//     grab->shsurf_destroy_listener.notify = destroy_shell_grab_shsurf;
//     wl_signal_add(&shsurf->resource.destroy_signal, &grab->shsurf_destroy_listener);

    struct wl_pointer *pointer = seat->seat.pointer;
    grab->pointer = pointer;
//     grab->grab.focus = &shsurf->m_surface->surface;

    wl_pointer_start_grab(pointer, &grab->grab);
    setGrabCursor(cursor);
    wl_pointer_set_focus(pointer, &m_grabSurface->surface, wl_fixed_from_int(0), wl_fixed_from_int(0));
}

void Shell::endGrab(ShellGrab *grab)
{
//     if (grab->shsurf)
//         wl_list_remove(&grab->shsurf_destroy_listener.link);

    wl_pointer_end_grab(grab->pointer);
}

static void configure_static_surface(struct weston_surface *es, Layer *layer, int32_t width, int32_t height)
{
    if (width == 0)
        return;

    for (struct weston_surface *s: *layer) {
        if (s->output == es->output && s != es) {
            weston_surface_unmap(s);
            s->configure = NULL;
        }
    }

    weston_surface_configure(es, es->output->x, es->output->y, width, height);

    if (wl_list_empty(&es->layer_link)) {
        layer->addSurface(es);
        weston_compositor_schedule_repaint(es->compositor);
    }
}

static void configure_panel_surface(struct weston_surface *es, Layer *layer,
                                    int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (width == 0)
        return;

    weston_surface_configure(es, x, y, width, height);

    if (wl_list_empty(&es->layer_link)) {
        layer->addSurface(es);
        weston_compositor_schedule_repaint(es->compositor);
    }
}

void Shell::backgroundConfigure(struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    configure_static_surface(es, &m_backgroundLayer, width, height);
}

void Shell::panelConfigure(struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    configure_panel_surface(es, &m_panelsLayer, es->geometry.x, es->geometry.y, width, height);
}

void Shell::setBackgroundSurface(struct weston_surface *surface, struct weston_output *output)
{
    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<Shell *>(es->configure_private)->backgroundConfigure(es, sx, sy, width, height); };
    surface->configure_private = this;
    surface->output = output;
}

void Shell::setGrabSurface(struct weston_surface *surface)
{
    m_grabSurface = surface;
}

void Shell::addPanelSurface(struct weston_surface *surface, struct weston_output *output)
{
    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<Shell *>(es->configure_private)->panelConfigure(es, sx, sy, width, height); };;
    surface->configure_private = this;
    surface->output = output;
}

void Shell::addLauncherSurface(struct weston_surface *surface, struct weston_output *output)
{
    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<Shell *>(es->configure_private)->panelConfigure(es, sx, sy, width, height); };;
    surface->configure_private = this;
    surface->output = output;
}

void Shell::addSpecialSurface(struct weston_surface *surface, struct weston_output *output)
{
    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<Shell *>(es->configure_private)->panelConfigure(es, sx, sy, width, height); };;
    surface->configure_private = this;
    surface->output = output;
}

void Shell::showPanels()
{
    m_panelsLayer.show();
}

void Shell::hidePanels()
{
    m_panelsLayer.hide();
}

struct weston_output *Shell::getDefaultOutput() const
{
    return container_of(m_compositor->output_list.next, struct weston_output, link);
}

Workspace *Shell::currentWorkspace() const
{
    return m_workspaces[m_currentWorkspace];
}

Workspace *Shell::workspace(uint32_t id) const
{
    if (id >= m_workspaces.size()) {
        return nullptr;
    }

    return m_workspaces[id];
}

void Shell::selectPreviousWorkspace()
{
    Workspace *old = currentWorkspace();
    if (--m_currentWorkspace < 0) {
        m_currentWorkspace = m_workspaces.size() - 1;
    }
    activateWorkspace(old);
}

void Shell::selectNextWorkspace()
{
    Workspace *old = currentWorkspace();
    if (++m_currentWorkspace == m_workspaces.size()) {
        m_currentWorkspace = 0;
    }
    activateWorkspace(old);
}

void Shell::selectWorkspace(uint32_t id)
{
    if (id >= m_workspaces.size()) {
        return;
    }

    Workspace *old = currentWorkspace();
    m_currentWorkspace = id;
    activateWorkspace(old);
}

void Shell::activateWorkspace(Workspace *old)
{
    if (old) {
        old->remove();
    }

    currentWorkspace()->insert(container_of(m_compositor->cursor_layer.link.next, struct weston_layer, link));
}

uint32_t Shell::numWorkspaces() const
{
    return m_workspaces.size();
}

void Shell::showAllWorkspaces()
{
    currentWorkspace()->remove();

    Workspace *prev = nullptr;
    for (Workspace *w: m_workspaces) {
        if (prev) {
            w->insert(prev);
        } else {
            w->insert(container_of(m_compositor->cursor_layer.link.next, struct weston_layer, link));
        }
        prev = w;
    }
}

void Shell::resetWorkspaces()
{
    for (Workspace *w: m_workspaces) {
        w->remove();
    }
    activateWorkspace(nullptr);
}

void Shell::pointerFocus(ShellSeat *, struct wl_pointer *pointer)
{
    struct weston_surface *surface = container_of(pointer->focus, struct weston_surface, surface);

    if (!surface)
        return;

    struct weston_compositor *compositor = surface->compositor;
    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (!shsurf)
        return;

    if (!shsurf->isResponsive()) {
        shsurf->shell()->setBusyCursor(shsurf, container_of(pointer->seat, struct weston_seat, seat));
    } else {
        uint32_t serial = wl_display_next_serial(compositor->wl_display);
        shsurf->ping(serial);
    }
}

void Shell::pingTimeout(ShellSurface *shsurf)
{
    struct weston_seat *seat;
    wl_list_for_each(seat, &m_compositor->seat_list, link) {
        if (seat->seat.pointer->focus == &shsurf->m_surface->surface)
            setBusyCursor(shsurf, seat);
    }
}

void Shell::pong(ShellSurface *shsurf)
{
    if (!shsurf->isResponsive()) {
        struct weston_seat *seat;
        /* Received pong from previously unresponsive client */
        wl_list_for_each(seat, &m_compositor->seat_list, link) {
            struct wl_pointer *pointer = seat->seat.pointer;
            if (pointer->focus == &m_grabSurface->surface && pointer->current == &shsurf->m_surface->surface) {
                endBusyCursor(seat);
            }
        }
    }
}

const struct wl_shell_interface Shell::shell_implementation = {
    [](struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface_resource) {
        static_cast<Shell *>(resource->data)->getShellSurface(client, resource, id, surface_resource);
    }
};

void Shell::bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    wl_client_add_object(client, &wl_shell_interface, &shell_implementation, id, this);
}

void Shell::sigchld(int status)
{
    uint32_t time;

    m_child.process.pid = 0;
    m_child.client = nullptr; /* already destroyed by wayland */

    /* if desktop-shell dies more than 5 times in 30 seconds, give up */
    time = weston_compositor_get_time();
    if (time - m_child.deathstamp > 30000) {
        m_child.deathstamp = time;
        m_child.deathcount = 0;
    }

    m_child.deathcount++;
    if (m_child.deathcount > 5) {
        weston_log("starthawaii died, giving up.\n");
        return;
    }

    weston_log("starthawaii died, respawning...\n");
    launchShellProcess();
}

IRect2D Shell::windowsArea(struct weston_output *output)
{
    return m_windowsArea[output];
}

void Shell::launchShellProcess()
{
    const char *shell_exe = INSTALL_LIBEXECDIR "/starthawaii";

    m_child.client = weston_client_launch(m_compositor,
                                          &m_child.process,
                                          shell_exe,
                                          [](struct weston_process *process, int status) {
                                              Child *child = container_of(process, Child, process);
                                              child->shell->sigchld(status);
                                          });

    printf("launch\n");

    if (!m_child.client)
        weston_log("not able to start %s\n", shell_exe);
}


WL_EXPORT int
module_init(struct weston_compositor *ec,
        int *argc, char *argv[], const char *config_file)
{
    Shell *shell = Shell::load<DesktopShell>(ec);
    if (!shell) {
        return -1;
    }
//     struct weston_seat *seat;
//     struct desktop_shell *shell;
//     struct wl_notification_daemon *notification;
//     struct workspace **pws;
//     unsigned int i;
//     struct wl_event_loop *loop;
//
//     shell = malloc(sizeof *shell);
//     if (shell == NULL)
//         return -1;
//     memset(shell, 0, sizeof *shell);
//     shell->compositor = ec;
//
//     notification = malloc(sizeof *notification);
//     if (notification == NULL)
//         return -1;
//     memset(notification, 0, sizeof *notification);
//     notification->compositor = ec;
//
//     shell->destroy_listener.notify = shell_destroy;
//     wl_signal_add(&ec->destroy_signal, &shell->destroy_listener);
//     shell->idle_listener.notify = idle_handler;
//     wl_signal_add(&ec->idle_signal, &shell->idle_listener);
//     shell->wake_listener.notify = wake_handler;
//     wl_signal_add(&ec->wake_signal, &shell->wake_listener);
//     shell->show_input_panel_listener.notify = show_input_panels;
//     wl_signal_add(&ec->show_input_panel_signal, &shell->show_input_panel_listener);
//     shell->hide_input_panel_listener.notify = hide_input_panels;
//     wl_signal_add(&ec->hide_input_panel_signal, &shell->hide_input_panel_listener);
//     ec->ping_handler = ping_handler;
//     ec->shell_interface.shell = shell;
//     ec->shell_interface.create_shell_surface = create_shell_surface;
//     ec->shell_interface.set_toplevel = set_toplevel;
//     ec->shell_interface.set_transient = set_transient;
//     ec->shell_interface.set_fullscreen = set_fullscreen;
//     ec->shell_interface.move = surface_move;
//     ec->shell_interface.resize = surface_resize;
//
//     wl_list_init(&shell->input_panel.surfaces);
//
//     weston_layer_init(&notification->layer, &ec->cursor_layer.link);
//     weston_layer_init(&shell->fullscreen_layer, &notification->layer.link);
//     weston_layer_init(&shell->panel_layer, &shell->fullscreen_layer.link);
//     weston_layer_init(&shell->background_layer, &shell->panel_layer.link);
//     weston_layer_init(&shell->lock_layer, NULL);
//     weston_layer_init(&shell->input_panel_layer, NULL);
//
//     wl_array_init(&shell->workspaces.array);
//     wl_list_init(&shell->workspaces.client_list);
//
//     shell_configuration(shell, notification, config_file);
//
//     for (i = 0; i < shell->workspaces.num; i++) {
//         pws = wl_array_add(&shell->workspaces.array, sizeof *pws);
//         if (pws == NULL)
//             return -1;
//
//         *pws = workspace_create();
//         if (*pws == NULL)
//             return -1;
//     }
//     activate_workspace(shell, 0);
//
//     wl_list_init(&shell->workspaces.anim_sticky_list);
//     wl_list_init(&shell->workspaces.animation.link);
//     shell->workspaces.animation.frame = animate_workspace_change_frame;
//
//     struct wl_global *global = wl_display_add_global(ec->wl_display, &wl_shell_interface, shell,
//                                [](struct wl_client *client, void *data, uint32_t version, uint32_t id) {
//                                    static_cast<Shell *>(data)->bind(client, version, id);
//                                });
//
//     if (!global)
//         return -1;
//
//     if (wl_display_add_global(ec->wl_display,
//                   &desktop_shell_interface,
//                   shell, bind_desktop_shell) == NULL)
//         return -1;
//
//     if (wl_display_add_global(ec->wl_display,
//                   &wl_notification_daemon_interface,
//                   notification, bind_notification_daemon) == NULL)
//         return -1;
//
//     if (wl_display_add_global(ec->wl_display, &screensaver_interface,
//                   shell, bind_screensaver) == NULL)
//         return -1;
//
//     if (wl_display_add_global(ec->wl_display, &input_panel_interface,
//                   shell, bind_input_panel) == NULL)
//         return -1;
//
//     if (wl_display_add_global(ec->wl_display, &workspace_manager_interface,
//                   shell, bind_workspace_manager) == NULL)
//         return -1;
//
//     shell->child.deathstamp = weston_compositor_get_time();
//
//     loop = wl_display_get_event_loop(ec->wl_display);
//     wl_event_loop_add_idle(loop, launch_desktop_shell_process, shell);
//
//     shell->screensaver.timer =
//         wl_event_loop_add_timer(loop, 0, shell);
//
//     wl_list_for_each(seat, &ec->seat_list, link)
//         create_pointer_focus_listener(seat);
//
//     shell_add_bindings(ec, shell);
//
//     shell_fade(shell, FADE_IN);

    return 0;
}
