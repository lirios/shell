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

#include "cmakedirs.h"
#include "screensaver.h"
#include "desktop-shell.h"

#include <signal.h>

ScreenSaver::ScreenSaver()
    : Interface()
    , m_binding(nullptr)
    , m_enabled(false)
    , m_path(INSTALL_LIBEXECDIR "/hawaii-screensaver")
    , m_duration(5*60*1000)
{
    m_child.screenSaver = this;
    m_child.process.pid = 0;

    wl_global_create(Shell::instance()->compositor()->wl_display, &wl_screensaver_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
        static_cast<ScreenSaver *>(data)->bind(client, version, id);
    });

    struct wl_event_loop *loop = wl_display_get_event_loop(Shell::instance()->compositor()->wl_display);
    m_timer = wl_event_loop_add_timer(loop, [](void *data) {
        return static_cast<ScreenSaver *>(data)->timeout(); }, this);
}

void ScreenSaver::launchProcess()
{
    if (m_binding)
        return;

    if (m_path.empty() || !m_enabled) {
        weston_compositor_sleep(Shell::instance()->compositor());
        return;
    }

    if (m_child.process.pid != 0) {
        weston_log("old screensaver still running\n");
        return;
    }

    m_child.client = weston_client_launch(Shell::instance()->compositor(),
                                          &m_child.process,
                                          m_path.c_str(),
                                          [](struct weston_process *process, int status) {
        ScreenSaverChild *child = container_of(process, ScreenSaverChild, process);
        child->screenSaver->sigChild(status);
    });

    if (!m_child.client)
        weston_log("not able to start %s\n", m_path.c_str());
}

void ScreenSaver::terminateProcess()
{
    if (m_child.process.pid == 0)
        return;

    ::kill(m_child.process.pid, SIGTERM);
}

void ScreenSaver::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &wl_screensaver_interface, version, id);

    if (m_binding) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "only one client is allowed to bind wl_screensaver");
        wl_resource_destroy(resource);
        return;
    }

    if (client != Shell::instance()->shellClient()) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "permission to bind wl_screensaver_interface denied");
        wl_resource_destroy(resource);
        return;
    }

    wl_resource_set_implementation(resource, &implementation, this,
                                   [](struct wl_resource *resource) {
        static_cast<ScreenSaver *>(resource->data)->unbind(resource);
    });

    m_binding = resource;
}

void ScreenSaver::unbind(wl_resource *resource)
{
    if (m_binding == resource)
        m_binding = nullptr;
    free(resource);
}

void ScreenSaver::sigChild(int)
{
    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());

    m_child.process.pid = 0;
    m_child.client = nullptr; // already destroyed by wayland

    if (shell->isLocked())
        weston_compositor_sleep(Shell::instance()->compositor());
}

int ScreenSaver::timeout()
{
    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());

    weston_log("screensaver timeout...\n");
    shell->fadeOut();
    return 1;
}

void ScreenSaver::configure(weston_surface *es)
{
    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());

    // Starting screensaver beforehand doesn't work
    if (!shell->isLocked())
        return;

    weston_view *view = container_of(es->views.next, weston_view, surface_link);

    shell->centerSurfaceOnOutput(view, es->output);

    if (wl_list_empty(&view->layer_link)) {
        shell->prependViewToLockLayer(view);
        weston_view_update_transform(view);
        wl_event_source_timer_update(m_timer, m_duration);
        shell->fadeIn();
    }
}

void ScreenSaver::setSurface(wl_client *client,
                             wl_resource *resource,
                             wl_resource *output_resource,
                             wl_resource *surface_resource)
{
    weston_output *output = static_cast<weston_output *>(output_resource->data);
    weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        static_cast<ScreenSaver *>(es->configure_private)->configure(es); };
    surface->configure_private = this;
    surface->output = output;
}

const struct wl_screensaver_interface ScreenSaver::implementation = {
    [](wl_client *client, wl_resource *resource, wl_resource *output_resource, wl_resource *surface_resource) {
        static_cast<ScreenSaver *>(wl_resource_get_user_data(resource))->setSurface(client, resource, output_resource, surface_resource);
    }
};
