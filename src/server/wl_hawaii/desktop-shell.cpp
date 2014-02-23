/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013-2014 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include <wayland-server.h>
#include <weston/compositor.h>
#include <weston/matrix.h>

#include "config.h"
#include "cmakedirs.h"
#include "desktop-shell.h"
#include "wayland-hawaii-server-protocol.h"
#include "wayland-notification-daemon-server-protocol.h"
#include "wayland-screensaver-server-protocol.h"
#include "shellsurface.h"
#include "binding.h"
#include "inputpanel.h"
#include "shellseat.h"
#include "workspace.h"
#include "wl_shell/wlshell.h"
#include "wl_shell/wlshellsurface.h"
#include "xwlshell.h"
#include "animation.h"
#include "settings.h"
#include "settingsinterface.h"
#include "sessionmanager.h"
#include "wl_hawaii/dropdown.h"
#include "wl_hawaii/hawaiiclientwindow.h"
#include "wl_hawaii/hawaiiworkspace.h"
#include "wl_hawaii/panelsurface.h"
#include "wl_hawaii/shellwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <linux/input.h>

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#  include <unistd.h>
#endif

class PopupGrab : public ShellGrab {
public:
    weston_view *view;
    wl_resource *shsurfResource;
    bool inside;
    uint32_t creationTime;

    PopupGrab()
        : ShellGrab()
    {
    }

    void end()
    {
        wl_hawaii_popup_surface_send_popup_done(shsurfResource);
        wl_resource_destroy(shsurfResource);
    }

    void focus() override
    {
        wl_fixed_t sx, sy;
        weston_view *pickedView = weston_compositor_pick_view(pointer()->seat->compositor,
                                                              pointer()->x, pointer()->y,
                                                              &sx, &sy);

        inside = pickedView == view;
        if (inside)
            weston_pointer_set_focus(pointer(), view, sx, sy);
    }

    void motion(uint32_t time, wl_fixed_t x, wl_fixed_t y) override
    {
        ShellGrab::motion(time, x, y);

        wl_resource *resource;
        wl_resource_for_each(resource, &pointer()->focus_resource_list) {
            wl_fixed_t sx, sy;
            weston_view_from_global_fixed(pointer()->focus, pointer()->x, pointer()->y, &sx, &sy);
            wl_pointer_send_motion(resource, time, sx, sy);
        }
    }

    void button(uint32_t time, uint32_t button, uint32_t state) override
    {
        wl_resource *resource;
        wl_resource_for_each(resource, &pointer()->focus_resource_list) {
            struct wl_display *display = wl_client_get_display(wl_resource_get_client(resource));
            uint32_t serial = wl_display_get_serial(display);
            wl_pointer_send_button(resource, serial, time, button, state);
        }

        // Make sure windows don't get shown or hidden too fast because of a QQuickWindow bug
        // that hangs the client process
        if (!inside && state == WL_POINTER_BUTTON_STATE_RELEASED && time - creationTime > 500)
            end();
    }
};

struct Popup {
    Popup(weston_view *p, DesktopShell *s, int32_t _x, int32_t _y)
        : parent(p)
        , shell(s)
        , x(_x)
        , y(_y)
        , grab(nullptr)
    {
    }

    ~Popup()
    {
        delete grab;
    }

    weston_view *parent;
    DesktopShell *shell;
    int32_t x, y;
    PopupGrab *grab;
    wl_listener destroyListener;
};

static void popupDestroyed(wl_listener *listener, void *data)
{
    weston_surface *surface = static_cast<weston_surface *>(data);
    delete static_cast<Popup *>(surface->configure_private);
}

class Splash
{
public:
    enum FadeType {
        FadeUnknown = 0,
        FadeIn,
        FadeOut
    };

    Splash(DesktopShell *shell)
        : m_shell(shell)
        , m_view(nullptr)
    {
        m_view = createSurface();

        m_splash.type = FadeUnknown;
        m_splash.parent = this;
        m_splash.fadeAnimation = new Animation();
        m_splash.fadeAnimation->updateSignal->connect(&m_splash, &splash::setAlpha);
        m_splash.fadeAnimation->doneSignal->connect(&m_splash, &splash::done);
    }

    ~Splash()
    {
        delete m_splash.fadeAnimation;

        if (m_view) {
            weston_surface_destroy(m_view->surface);
            weston_view_destroy(m_view);
        }
    }

    weston_view *createSurface()
    {
        // Create a surface big enough to cover all screens
        weston_surface *surface = weston_surface_create(m_shell->compositor());
        if (!surface)
            return nullptr;
        weston_view *view = weston_view_create(surface);
        if (!view) {
            weston_surface_destroy(surface);
            return nullptr;
        }

        weston_surface_set_size(surface, 8192, 8192);
        weston_view_set_position(view, 0, 0);
        weston_surface_set_color(surface, 0.0, 0.0, 0.0, 1.0);
        wl_list_insert(&m_shell->compositor()->fade_layer.view_list,
                       &view->layer_link);
        pixman_region32_init(&surface->input);

        return view;
    }

    void fadeIn()
    {
        if (!m_view)
            m_view = createSurface();

        m_splash.type = FadeIn;
        m_splash.fadeAnimation->setStart(0.f);
        m_splash.fadeAnimation->setTarget(1.f);
        m_splash.fadeAnimation->run(m_shell->getDefaultOutput(), 250, Animation::Flags::SendDone);
    }

    void fadeOut()
    {
        if (!m_view)
            return;

        m_splash.type = FadeOut;
        m_splash.fadeAnimation->setStart(1.f);
        m_splash.fadeAnimation->setTarget(0.f);
        m_splash.fadeAnimation->run(m_shell->getDefaultOutput(), 250, Animation::Flags::SendDone);
    }

private:
    struct splash {
        Splash::FadeType type;
        Splash *parent;
        Animation *fadeAnimation;

        void setAlpha(float a)
        {
            parent->m_view->alpha = a;
            weston_view_geometry_dirty(parent->m_view);
            weston_surface_damage(parent->m_view->surface);
        }

        void done()
        {
            switch (type) {
            case Splash::FadeIn:
                parent->m_shell->lockSession();
                break;
            case Splash::FadeOut:
                weston_surface_destroy(parent->m_view->surface);
                parent->m_view = nullptr;
                break;
            default:
                break;
            }
        }
    };

    friend class splash;

    DesktopShell *m_shell;
    weston_view *m_view;
    splash m_splash;
};

struct Client {
    ~Client() {
        destroyListener.reset();
    }

    wl_client *client;
    WlListener destroyListener;
};

DesktopShell::DesktopShell(struct weston_compositor *ec)
    : Shell(ec)
    , m_inputPanel(nullptr)
    , m_splash(nullptr)
    , m_sessionManager(nullptr)
    , m_screenSaverBinding(nullptr)
    , m_screenSaverEnabled(false)
    , m_screenSaverPath(INSTALL_LIBEXECDIR "/hawaii-screensaver")
    , m_screenSaverDuration(5*60*1000)
    , m_panelManagerBinding(nullptr)
    , m_notificationsEdge(DesktopShell::EdgeRight)
    , m_notificationsCornerAnchor(DesktopShell::CornerTopRight)
    , m_notificationsOrder(DesktopShell::OrderNewestFirst)
    , m_notificationsMargin(20)
    , m_prepareEventSent(false)
    , m_locked(false)
    , m_lockSurface(nullptr)
{
    m_screenSaverChild.shell = this;
    m_screenSaverChild.process.pid = 0;
}

DesktopShell::~DesktopShell()
{
    delete m_splash;

    for (auto value: m_trustedClients) {
        for (Client *c: value.second)
            delete c;
    }

    delete m_moveBinding;
    delete m_resizeBinding;
    delete m_closeBinding;
    delete m_prevWsBinding;
    delete m_nextWsBinding;
    delete m_quitBinding;

    if (m_sessionManager) {
        std::list<pid_t> pids;
        for (ShellSurface *s: surfaces()) {
            WlShellSurface *w = s->findInterface<WlShellSurface>();
            if (w) {
                wl_client *c = wl_resource_get_client(w->resource());
                pid_t pid;
                wl_client_get_credentials(c, &pid, nullptr, nullptr);
                pids.push_back(pid);
            }
        }
        m_sessionManager->save(pids);
    }
}

void DesktopShell::init()
{
    Shell::init();

    if (!wl_global_create(compositor()->wl_display, &wl_notification_daemon_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindNotifications(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &wl_screensaver_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindScreenSaver(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &wl_hawaii_panel_manager_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindPanelManager(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &wl_hawaii_shell_surface_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindDesktopShellSurface(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &wl_hawaii_shell_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindDesktopShell(client, version, id); }))
        return;

    m_idleListener.listen(&compositor()->idle_signal);
    m_idleListener.signal->connect(this, &DesktopShell::idle);

    m_wakeListener.listen(&compositor()->wake_signal);
    m_wakeListener.signal->connect(this, &DesktopShell::wake);

    struct wl_event_loop *loop = wl_display_get_event_loop(compositor()->wl_display);
    m_screenSaverTimer = wl_event_loop_add_timer(loop, [](void *data) {
        return static_cast<DesktopShell *>(data)->screenSaverTimeout(); }, this);

    m_moveBinding = new Binding();
    m_moveBinding->buttonTriggered.connect(this, &DesktopShell::moveBinding);
    m_resizeBinding = new Binding();
    m_resizeBinding->buttonTriggered.connect(this, &DesktopShell::resizeBinding);
    m_closeBinding = new Binding();
    m_closeBinding->keyTriggered.connect(this, &DesktopShell::closeBinding);
    m_prevWsBinding = new Binding();
    m_prevWsBinding->keyTriggered.connect([this](weston_seat *seat, uint32_t time, uint32_t key) {
        selectPreviousWorkspace();
    });
    m_nextWsBinding = new Binding();
    m_nextWsBinding->keyTriggered.connect([this](weston_seat *seat, uint32_t time, uint32_t key) {
        selectNextWorkspace();
    });
    m_quitBinding = new Binding();
    m_quitBinding->keyTriggered.connect([this](weston_seat *seat, uint32_t time, uint32_t key) {
        Shell::quit();
    });

    WlShell *wls = new WlShell;
    wls->surfaceResponsivenessChangedSignal.connect(this, &DesktopShell::surfaceResponsivenessChanged);
    addInterface(wls);
    addInterface(new XWlShell);
    addInterface(new SettingsInterface);

    m_inputPanel = new InputPanel(compositor()->wl_display);
    m_splash = new Splash(this);
}

weston_view *DesktopShell::createBlackSurfaceWithInput(int x, int y, int w, int h, float a)
{
    weston_surface *surface = weston_surface_create(compositor());
    weston_view *view = weston_view_create(surface);

    surface->configure = [](weston_surface *, int32_t, int32_t) {
        // nop
    };
    surface->configure_private = 0;
    weston_surface_set_color(surface, 0.0, 0.0, 0.0, 1);

    surface->width = w;
    surface->height = h;
    weston_view_set_position(view, x, y);

    return view;
}

void DesktopShell::fadeIn()
{
    m_splash->fadeOut();
}

void DesktopShell::fadeOut()
{
    m_splash->fadeIn();
}

void DesktopShell::lockSession()
{
    if (m_locked) {
        weston_compositor_sleep(compositor());
        return;
    }

    m_locked = true;

    // Change layers order so the lock screen is shown for the primary
    // output and backgrounds are show for the other outputs.
    // Application windows, panels, overlays, etc... are hidden under
    // the backgrounds layer.
    m_lockLayer.insert(&compositor()->cursor_layer);
    m_backgroundLayer.insert(&m_lockLayer);
    currentWorkspace()->insert(&m_backgroundLayer);

    // TODO: Disable bindings that are not supposed to work while locked

    // Run screensaver or sleep
    launchScreenSaverProcess();

    // All this must be undone in resumeDesktop()
}

void DesktopShell::unlockSession()
{
    weston_log("unlocking session...\n");

    if (!m_locked || m_lockSurface) {
        fadeIn();
        return;
    }

    // If the client has gone away, unlock immediately
    if (!shellClientResource()) {
        resumeDesktop();
        return;
    }

    if (m_prepareEventSent)
        return;

    weston_log("preparing lock surface...\n");
    weston_compositor_damage_all(compositor());
    wl_hawaii_shell_send_prepare_lock_surface(shellClientResource());
    m_prepareEventSent = true;
}

void DesktopShell::resumeDesktop()
{
    terminateScreenSaverProcess();

    // Restore layers order
    m_lockLayer.insert(&compositor()->cursor_layer);
    m_backgroundLayer.insert(&m_limboLayer);
    currentWorkspace()->insert(&m_limboLayer);

    m_locked = false;
    fadeIn();
    weston_compositor_damage_all(compositor());
}

void DesktopShell::setGrabCursor(Cursor cursor)
{
    wl_hawaii_shell_send_grab_cursor(m_child.desktop_shell, (uint32_t)cursor);
}

ShellSurface *DesktopShell::createShellSurface(weston_surface *surface, const weston_shell_client *client)
{
    ShellSurface *s = Shell::createShellSurface(surface, client);
    s->addInterface(new HawaiiClientWIndow);
    return s;
}

struct BusyGrab : public ShellGrab {
    void focus() override
    {
        wl_fixed_t sx, sy;
        weston_view *view = weston_compositor_pick_view(pointer()->seat->compositor, pointer()->x, pointer()->y, &sx, &sy);

        if (surface->view() != view) {
            delete this;
        }
    }
    void button(uint32_t time, uint32_t button, uint32_t state) override
    {
        weston_seat *seat = pointer()->seat;

        if (surface && button == BTN_LEFT && state) {
            ShellSeat::shellSeat(seat)->activate(surface);
            surface->dragMove(seat);
        } else if (surface && button == BTN_RIGHT && state) {
            ShellSeat::shellSeat(seat)->activate(surface);
            //         surface_rotate(grab->surface, &seat->seat);
        }
    }

    ShellSurface *surface;
};

void DesktopShell::setBusyCursor(ShellSurface *surface, struct weston_seat *seat)
{
    BusyGrab *grab = new BusyGrab;
    if (!grab && grab->pointer())
        return;

    grab->surface = surface;
    grab->start(seat, Cursor::Busy);
}

void DesktopShell::endBusyCursor(struct weston_seat *seat)
{
    ShellGrab *grab = ShellGrab::fromGrab(seat->pointer->grab);

    if (dynamic_cast<BusyGrab *>(grab)) {
        delete grab;
    }
}

bool DesktopShell::isTrusted(wl_client *client, const char *interface) const
{
    if (client == m_child.client)
        return true;

    auto it = m_trustedClients.find(interface);
    if (it == m_trustedClients.end())
        return false;

    for (Client *c: it->second) {
        if (c->client == client)
            return true;
    }

    return false;
}

IRect2D DesktopShell::windowsArea(struct weston_output *output) const
{
    for (Output o: m_outputs) {
        if (o.output == output)
            return o.rect;
    }

    return Shell::windowsArea(output);
}

void DesktopShell::trustedClientDestroyed(void *data)
{
    wl_client *client = static_cast<wl_client *>(data);
    for (auto v: m_trustedClients) {
        std::list<Client *> &list = m_trustedClients[v.first];
        for (auto i = list.begin(); i != list.end(); ++i) {
            if ((*i)->client == client) {
                delete *i;
                list.erase(i);
                return;
            }
        }
    }
}

void DesktopShell::idle(void *)
{
    weston_log("idle...\n");
    fadeOut();
}

void DesktopShell::wake(void *)
{
    weston_log("wake...\n");
    unlockSession();
}

void DesktopShell::sendInitEvents()
{
    for (uint i = 0; i < numWorkspaces(); ++i) {
        Workspace *ws = workspace(i);
        HawaiiWorkspace *dws = ws->findInterface<HawaiiWorkspace>();
        dws->init(m_child.client);
        workspaceAdded(dws);
    }

    for (ShellSurface *shsurf: surfaces()) {
        HawaiiClientWIndow *w = shsurf->findInterface<HawaiiClientWIndow>();
        if (w) {
            w->create();
        }
    }

    m_outputs.clear();
    weston_output *out;
    wl_list_for_each(out, &compositor()->output_list, link) {
        wl_resource *resource;
        wl_resource_for_each(resource, &out->resource_list) {
            if (wl_resource_get_client(resource) == m_child.client) {
                IRect2D rect(out->x, out->y, out->width, out->height);
                m_outputs.push_back({ out, resource, rect });
            }
        }
    }
}

void DesktopShell::centerSurfaceOnOutput(weston_view *ev, weston_output *output)
{
    float x = output->x + (output->width - ev->surface->width) / 2;
    float y = output->y + (output->height - ev->surface->height) / 2;

    weston_view_set_position(ev, x, y);
}

void DesktopShell::workspaceAdded(HawaiiWorkspace *ws)
{
    wl_hawaii_shell_send_workspace_added(m_child.desktop_shell, ws->resource(), ws->workspace()->isActive());
}

void DesktopShell::surfaceResponsivenessChanged(ShellSurface *shsurf, bool responsiveness)
{
    weston_seat *seat;
    wl_list_for_each(seat, &compositor()->seat_list, link) {
        if (seat->pointer->focus == shsurf->view()) {
            responsiveness ? endBusyCursor(seat) : setBusyCursor(shsurf, seat);
        }
    }
}

void DesktopShell::bindNotifications(wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_notification_daemon_interface, version, id);

    if (resource) {
        wl_resource_set_implementation(resource, &m_notificationDaemonImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindNotifications(resource); });
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_notification_daemon denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindNotifications(struct wl_resource *resource)
{
    free(resource);
}

void DesktopShell::bindDesktopShell(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_hawaii_shell_interface, version, id);

    if (client == m_child.client) {
        wl_resource_set_implementation(resource, &m_desktopShellImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindDesktopShell(resource); });
        m_child.desktop_shell = resource;

        sendInitEvents();
        wl_hawaii_shell_send_loaded(resource);
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_hawaii_shell denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindDesktopShell(struct wl_resource *resource)
{
    if (m_locked)
        resumeDesktop();

    m_child.desktop_shell = nullptr;
    m_prepareEventSent = false;
}

void DesktopShell::bindDesktopShellSurface(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_hawaii_shell_surface_interface, version, id);

    wl_resource_set_implementation(resource, &m_shellSurfaceImpl, this,
                                   [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindDesktopShellSurface(resource); });
    m_shellSurfaceBindings.push_back(resource);
}

void DesktopShell::unbindDesktopShellSurface(struct wl_resource *resource)
{
    m_shellSurfaceBindings.remove(resource);
}

void DesktopShell::bindPanelManager(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_hawaii_panel_manager_interface, version, id);

    if (m_panelManagerBinding) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "only one client is allowed to bind wl_hawaii_panel_manager");
        wl_resource_destroy(resource);
        return;
    }

    if (client == m_child.client) {
        wl_resource_set_implementation(resource, &m_panelManagerImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindPanelManager(resource); });

        m_panelManagerBinding = resource;
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_hawaii_panel_manager denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindPanelManager(struct wl_resource *resource)
{
    m_panelManagerBinding = nullptr;
}

void DesktopShell::bindScreenSaver(wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_screensaver_interface, version, id);

    if (!m_screenSaverBinding) {
        wl_resource_set_implementation(resource, &m_screenSaverImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindScreenSaver(resource); });
        m_screenSaverBinding = resource;
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_notification_daemon denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindScreenSaver(struct wl_resource *resource)
{
    m_screenSaverBinding = nullptr;
}

void DesktopShell::moveBinding(struct weston_seat *seat, uint32_t time, uint32_t button)
{
    weston_view *view = seat->pointer->focus;
    if (!view) {
        return;
    }

    ShellSurface *shsurf = getShellSurface(view->surface);
    if (!shsurf || shsurf->type() == ShellSurface::Type::Fullscreen || shsurf->type() == ShellSurface::Type::Maximized) {
        return;
    }

    shsurf = shsurf->topLevelParent();
    if (shsurf) {
        shsurf->dragMove(seat);
    }
}

void DesktopShell::resizeBinding(weston_seat *seat, uint32_t time, uint32_t button)
{
    weston_surface *surface = weston_surface_get_main_surface(seat->pointer->focus->surface);
    if (!surface) {
        return;
    }

    ShellSurface *shsurf = getShellSurface(surface);
    if (!shsurf || shsurf->type() == ShellSurface::Type::Fullscreen || shsurf->type() == ShellSurface::Type::Maximized) {
        return;
    }

    if (ShellSurface *top = shsurf->topLevelParent()) {
        int32_t x, y;
        weston_view_from_global(shsurf->view(), wl_fixed_to_int(seat->pointer->grab_x), wl_fixed_to_int(seat->pointer->grab_y), &x, &y);

        pixman_box32_t *bbox = pixman_region32_extents(&surface->input);

        ShellSurface::Edges edges = ShellSurface::Edges::None;
        int32_t w = surface->width / 3;
        if (w > 20) w = 20;
        w += bbox->x1;

        if (x < w)
            edges |= ShellSurface::Edges::Left;
        else if (x < surface->width - w)
            edges |= ShellSurface::Edges::None;
        else
            edges |= ShellSurface::Edges::Right;

        int32_t h = surface->height / 3;
        if (h > 20) h = 20;
        h += bbox->y1;
        if (y < h)
            edges |= ShellSurface::Edges::Top;
        else if (y < surface->height - h)
            edges |= ShellSurface::Edges::None;
        else
            edges |= ShellSurface::Edges::Bottom;

        top->dragResize(seat, edges);
    }
}

void DesktopShell::closeBinding(weston_seat *seat, uint32_t time, uint32_t button)
{
    weston_surface *surface = weston_surface_get_main_surface(seat->keyboard->focus);
    if (!surface) {
        return;
    }

    ShellSurface *shsurf = getShellSurface(surface);
    if (!shsurf || shsurf->type() == ShellSurface::Type::Fullscreen || shsurf->type() == ShellSurface::Type::Maximized) {
        return;
    }

    shsurf->close();
}

#if 0
class Switcher
{
public:
    Switcher()
    {
    }

    void destroy()
    {
        weston_keyboard *keyboard = grab.keyboard;

        for (weston_surface *surface: shell->currentWorkspace()->layer()) {
            surface->alpha = 1.f;
            weston_surface_damage(surface);
        }

        if (current) {
            ShellSurface *shsurf = shell->getShellSurface(current);
            if (shsurf)
                shsurf->activate();
        }
        wl_list_remove(&listener.link);
        weston_keyboard_end_grab(keyboard);
        if (keyboard->input_method_resource)
            keyboard->grab = &keyboard->input_method_grab;

        wl_hawaii_shell_send_window_switching_finished(shell->shellClientResource());
    }

    void switchNext()
    {
        wl_hawaii_shell_send_window_switching_started(shell->shellClientResource());

        weston_surface *first = nullptr, *prev = nullptr, *next = nullptr;

        for (weston_surface *surface: shell->currentWorkspace()->layer()) {
            ShellSurface *shsurf = shell->getShellSurface(surface);
            if (!shsurf)
                continue;

            switch (shsurf->type()) {
            case ShellSurface::Type::TopLevel:
            case ShellSurface::Type::Fullscreen:
            case ShellSurface::Type::Maximized:
                if (!first)
                    first = surface;
                if (prev == current)
                    next = surface;
                prev = surface;
                surface->alpha = 0.25f;
                weston_surface_geometry_dirty(surface);
                weston_surface_damage(surface);
                break;
            default:
                break;
            }

            if (shell->isBlackSurface(surface, nullptr)) {
                surface->alpha = 0.25f;
                weston_surface_geometry_dirty(surface);
                weston_surface_damage(surface);
            }
        }

        if (!next)
            next = first;
        if (!next)
            return;

        wl_list_remove(&listener.link);
        wl_signal_add(&next->destroy_signal, &listener);

        current = next;
        next->alpha = 1.f;

        ShellSurface *shsurf = shell->getShellSurface(next);
        if (shsurf)
            wl_hawaii_shell_send_window_switched(shell->shellClientResource(),
                                                 shsurf->windowResource());
    }

    static void handleSurfaceDestroy(wl_listener *l, void *data)
    {
        Switcher *switcher = container_of(l, Switcher, listener);
        switcher->switchNext();
    }

    DesktopShell *shell;
    weston_surface *current;
    wl_listener listener;
    weston_keyboard_grab grab;
};

static void switcherKey(weston_keyboard_grab *grab,
                        uint32_t time, uint32_t key, uint32_t state_w)
{
    Switcher *switcher = container_of(grab, Switcher, grab);
    enum wl_keyboard_key_state state = (enum wl_keyboard_key_state)state_w;

    if (key == KEY_TAB && state == WL_KEYBOARD_KEY_STATE_PRESSED)
        switcher->switchNext();
}

static void switcherModifier(weston_keyboard_grab *grab,
                             uint32_t serial, uint32_t mods_depressed,
                             uint32_t mods_latched, uint32_t mods_locked,
                             uint32_t group)
{
    Switcher *switcher = container_of(grab, Switcher, grab);
    weston_seat *seat = grab->keyboard->seat;

    if ((seat->modifier_state & MODIFIER_SUPER) == 0) {
        switcher->destroy();
        delete switcher;
    }
}

static const struct weston_keyboard_grab_interface switcherGrab = {
    switcherKey,
    switcherModifier
};
#endif

void DesktopShell::switcherBinding(weston_seat *seat, uint32_t time, uint32_t button)
{
#if 0
    Switcher *switcher = new Switcher();
    switcher->shell = this;
    switcher->current = nullptr;
    switcher->listener.notify = Switcher::handleSurfaceDestroy;
    wl_list_init(&switcher->listener.link);

    switcher->grab.interface = &switcherGrab;
    weston_keyboard_start_grab(seat->keyboard, &switcher->grab);
    weston_keyboard_set_focus(seat->keyboard, nullptr);
    switcher->switchNext();
#endif
}

void DesktopShell::addTrustedClient(wl_client *client, wl_resource *resource, int32_t fd, const char *interface)
{
    wl_client *c = wl_client_create(compositor()->wl_display, fd);

    Client *cl = new Client;
    cl->client = c;
    cl->destroyListener.signal->connect(this, &DesktopShell::trustedClientDestroyed);
    wl_client_add_destroy_listener(c, cl->destroyListener.listener());

    m_trustedClients[interface].push_back(cl);
}

void DesktopShell::addKeyBinding(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t key, uint32_t modifiers)
{
    wl_resource *res = wl_resource_create(client, &wl_hawaii_key_binding_interface, wl_resource_get_version(resource), id);
    wl_resource_set_implementation(res, nullptr, res, [](wl_resource *) {});

    weston_compositor_add_key_binding(compositor(), key, (weston_keyboard_modifier)modifiers,
                                      [](struct weston_seat *seat, uint32_t time, uint32_t key, void *data) {

        wl_hawaii_key_binding_send_triggered(static_cast<wl_resource *>(data));
    }, res);
}

void DesktopShell::lockSurfaceDestroy(void *)
{
    weston_log("lock surface gone\n");
    m_lockSurface = nullptr;
}

void DesktopShell::setAvailableGeometry(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *output_resource,
                                        int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct weston_output *output = static_cast<weston_output *>(output_resource->data);

    IRect2D area(x, y, width, height);
    //m_windowsArea[output] = area;
}

static void configure_static_view_no_position(weston_view *ev, Layer *layer)
{
    if (wl_list_empty(&ev->layer_link)) {
        layer->addSurface(ev);
        weston_compositor_schedule_repaint(ev->surface->compositor);
    }
}

static void configure_static_view(weston_view *ev, Layer *layer)
{
    weston_view_set_position(ev, ev->output->x, ev->output->y);
    configure_static_view_no_position(ev, layer);
}

void DesktopShell::setBackground(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                 struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);
    view->output = static_cast<weston_output *>(output_resource->data);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        DesktopShell *shell = static_cast<DesktopShell *>(es->configure_private);
        weston_view *view = container_of(es->views.next, weston_view, surface_link);
        configure_static_view(view, &shell->m_backgroundLayer);
    };
    surface->configure_private = this;
    surface->output = view->output;
}

void DesktopShell::setDesktop(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                              struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);
    view->output = static_cast<weston_output *>(output_resource->data);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        DesktopShell *shell = static_cast<DesktopShell *>(es->configure_private);
        weston_view *view = container_of(es->views.next, weston_view, surface_link);
        configure_static_view(view, &shell->m_desktopLayer);
    };
    surface->configure_private = this;
    surface->output = view->output;
}

void DesktopShell::setOverlay(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource, struct wl_resource *surface_resource)
{
    weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        DesktopShell *shell = static_cast<DesktopShell *>(es->configure_private);
        weston_view *view = container_of(es->views.next, weston_view, surface_link);
        configure_static_view(view, &shell->m_overlayLayer);
    };
    surface->configure_private = this;
    surface->output = static_cast<weston_output *>(output_resource->data);

    pixman_region32_fini(&surface->pending.input);
    pixman_region32_init_rect(&surface->pending.input, 0, 0, 0, 0);
}

void DesktopShell::configurePopup(weston_surface *es, int32_t sx, int32_t sy)
{
    if (es->width == 0)
        return;

    Popup *p = static_cast<Popup *>(es->configure_private);
    DesktopShell *shell = p->shell;
    Layer *layer = &shell->m_panelsLayer;

    weston_view *view = container_of(es->views.next, weston_view, surface_link);

    // Popup coordinates are relative to parent
    int32_t x = p->parent->geometry.x + p->x;
    int32_t y = p->parent->geometry.y + p->y;

    // But can't exceed output
    int32_t limitX = p->parent->output->x + p->parent->output->width;
    if (x + view->surface->width > limitX)
        x = limitX - view->surface->width;
    int32_t limitY = p->parent->output->y + p->parent->output->height;
    if (y + view->surface->height > limitY)
        y = limitY - view->surface->height;

    // Set view position
    weston_view_set_position(view, x, y);

    if (wl_list_empty(&view->layer_link) || view->layer_link.next == view->layer_link.prev) {
        layer->addSurface(view);
        weston_compositor_schedule_repaint(es->compositor);
    }
}

void DesktopShell::setPopup(struct wl_client *client, struct wl_resource *resource,
                            uint32_t id,
                            struct wl_resource *parent_resource,
                            struct wl_resource *surface_resource,
                            int32_t x, int32_t y)
{
    weston_surface *parent = static_cast<weston_surface *>(wl_resource_get_user_data(parent_resource));
    weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));
    weston_view *pv = container_of(parent->views.next, weston_view, surface_link);

    Popup *p = nullptr;
    if (surface->configure == configurePopup) {
        p = static_cast<Popup *>(surface->configure_private);
        p->x = x;
        p->y = y;
        p->parent = pv;
    } else {
        p = new Popup(pv, this, x, y);
        p->destroyListener.notify = popupDestroyed;
        wl_signal_add(&surface->destroy_signal, &p->destroyListener);
    }

    surface->configure = configurePopup;
    surface->configure_private = p;
    surface->output = parent->output;

    weston_view *sv = weston_view_create(surface);

    PopupGrab *grab = new PopupGrab;
    p->grab = grab;
    if (!grab)
        return;

    grab->shsurfResource = wl_resource_create(client, &wl_hawaii_popup_surface_interface, wl_resource_get_version(resource), id);
    wl_resource_set_user_data(grab->shsurfResource, grab);

    weston_seat *seat = container_of(compositor()->seat_list.next, weston_seat, link);
    grab->view = sv;
    grab->creationTime = seat->pointer->grab_time;

    wl_fixed_t sx, sy;
    weston_view_from_global_fixed(sv, seat->pointer->x, seat->pointer->y, &sx, &sy);
    weston_pointer_set_focus(seat->pointer, sv, sx, sy);

    grab->start(seat);
}

void DesktopShell::setDialog(struct wl_client *client, struct wl_resource *resource,
                             struct wl_resource *output_resource,
                             struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        ShellWindow *window = static_cast<ShellWindow *>(es->configure_private);
        if (!window)
            return;

        weston_view *view = container_of(es->views.next, weston_view, surface_link);

        if (wl_list_empty(&view->layer_link) || view->layer_link.next == view->layer_link.prev) {
            // Create a black translucent surface to prevent underlying layers to get input events
            if (!window->dimmedSurface()) {
                window->createDimmedSurface(es->output);
                window->shell()->m_dialogsLayer.addSurface(window->dimmedSurface());
            }

            // Add the dialog surface and stack it above the dimmed surface
            window->shell()->m_dialogsLayer.addSurface(view);
            window->shell()->m_dialogsLayer.stackAbove(view, window->dimmedSurface());
        }

        if (es->output)
            window->shell()->centerSurfaceOnOutput(view, es->output);
        //window->animateDialog(view);

        // Give focus to the dialog
        weston_seat *seat = container_of(es->compositor->seat_list.next, weston_seat, link);
        weston_surface_activate(es, seat);
    };
    surface->output = static_cast<weston_output *>(output_resource->data);

    ShellWindow *window = new ShellWindow(this);
    window->connectSignal(&surface->destroy_signal);
    surface->configure_private = window;
}

void DesktopShell::setPosition(struct wl_client *client, struct wl_resource *resource,
                               struct wl_resource *surface_resource,
                               int32_t x, int32_t y)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (!surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role not yet assigned");
        return;
    }

    // Create view and set given position
    weston_view *view = container_of(surface->views.next, weston_view, surface_link);
    if (view)
        weston_view_set_position(view, x, y);
}

void DesktopShell::setConfigSurface(wl_client *client,
                                    wl_resource *resource,
                                    wl_resource *output_resource,
                                    wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);

    if (surface->configure) {
        weston_view *view = container_of(surface->views.next, weston_view, surface_link);
        view->output = static_cast<weston_output *>(output_resource->data);
        surface->output = view->output;
    } else {
        weston_view *view, *next;
        wl_list_for_each_safe(view, next, &surface->views, surface_link)
                weston_view_destroy(view);
        view = weston_view_create(surface);
        view->output = static_cast<weston_output *>(output_resource->data);

        surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
            DesktopShell *shell = static_cast<DesktopShell *>(es->configure_private);
            weston_view *view = container_of(es->views.next, weston_view, surface_link);
            configure_static_view_no_position(view, &shell->m_panelsLayer);
        };
        surface->configure_private = this;
        surface->output = view->output;
    }
}

void DesktopShell::lockSurfaceConfigure(weston_surface *es, int32_t sx, int32_t sy)
{
    weston_view *view = container_of(es->views.next, weston_view, surface_link);

    if (wl_list_empty(&view->layer_link) || view->layer_link.next == view->layer_link.prev) {
        m_lockLayer.addSurface(view);
        weston_view_update_transform(view);
        fadeIn();
    }
}

void DesktopShell::setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    m_prepareEventSent = false;

    if (!m_locked)
        return;

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);

    m_lockSurface = view;

    m_lockSurfaceDestroyListener.listen(&surface->destroy_signal);
    m_lockSurfaceDestroyListener.signal->connect(this, &DesktopShell::lockSurfaceDestroy);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        static_cast<DesktopShell *>(es->configure_private)->lockSurfaceConfigure(es, sx, sy); };
    surface->configure_private = this;
    surface->output = nullptr;
}

void DesktopShell::quit(wl_client *client, wl_resource *resource)
{
    Shell::quit();
}

void DesktopShell::lock(wl_client *client, wl_resource *resource)
{
    // Don't lock again if we are already locked
    if (m_locked || m_lockSurface)
        return;

    wl_signal_emit(&compositor()->idle_signal, nullptr);
}

void DesktopShell::unlock(struct wl_client *client, struct wl_resource *resource)
{
    m_prepareEventSent = false;

    if (m_locked)
        resumeDesktop();
}

void DesktopShell::setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
    this->Shell::setGrabSurface(static_cast<struct weston_surface *>(wl_resource_get_user_data(surface_resource)));
}

void DesktopShell::desktopReady(struct wl_client *client, struct wl_resource *resource)
{
    if (m_sessionManager)
        m_sessionManager->restore();
    fadeIn();
}

void DesktopShell::addWorkspace(wl_client *client, wl_resource *resource)
{
    Workspace *ws = new Workspace(this, numWorkspaces());
    HawaiiWorkspace *dws = new HawaiiWorkspace;
    ws->addInterface(dws);
    dws->init(client);
    Shell::addWorkspace(ws);
    workspaceAdded(dws);
}

void DesktopShell::selectWorkspace(wl_client *client, wl_resource *resource, wl_resource *workspace_resource)
{
    Shell::selectWorkspace(HawaiiWorkspace::fromResource(workspace_resource)->workspace()->number());
}

const struct wl_hawaii_shell_interface DesktopShell::m_desktopShellImpl = {
    wrapInterface(&DesktopShell::addTrustedClient),
    wrapInterface(&DesktopShell::addKeyBinding),
    wrapInterface(&DesktopShell::setAvailableGeometry),
    wrapInterface(&DesktopShell::setBackground),
    wrapInterface(&DesktopShell::setDesktop),
    wrapInterface(&DesktopShell::setConfigSurface),
    wrapInterface(&DesktopShell::setLockSurface),
    wrapInterface(&DesktopShell::setGrabSurface),
    wrapInterface(&DesktopShell::setPosition),
    wrapInterface(&DesktopShell::quit),
    wrapInterface(&DesktopShell::lock),
    wrapInterface(&DesktopShell::unlock),
    wrapInterface(&DesktopShell::desktopReady),
    wrapInterface(&Shell::minimizeWindows),
    wrapInterface(&Shell::restoreWindows),
    wrapInterface(&DesktopShell::addWorkspace),
    wrapInterface(&DesktopShell::selectWorkspace)
};

const struct wl_hawaii_shell_surface_interface DesktopShell::m_shellSurfaceImpl = {
    wrapInterface(&DesktopShell::setOverlay),
    wrapInterface(&DesktopShell::setPopup),
    wrapInterface(&DesktopShell::setDialog)
};

void DesktopShell::setPanelSurface(struct wl_client *client, struct wl_resource *resource,
                                   uint32_t id,
                                   struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    PanelSurface *panel = new PanelSurface(client, resource, id);
    panel->view = weston_view_create(surface);
    panel->shell = this;
    m_panels.push_back(panel);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        PanelSurface *panel = static_cast<PanelSurface *>(es->configure_private);
        if (panel) {
            configure_static_view_no_position(panel->view, &panel->shell->m_panelsLayer);
            panel->setPosition();
            panel->shell->recalculateAvailableGeometry();
        }
    };
    surface->configure_private = panel;
    surface->output = nullptr;
}

void DesktopShell::recalculateAvailableGeometry()
{
    for (std::list<Output>::iterator o = m_outputs.begin(); o != m_outputs.end(); ++o) {
        IRect2D rect((*o).output->x, (*o).output->y,
                     (*o).output->width, (*o).output->height);

        for (PanelSurface *panel: m_panels) {
            // Ignore non-docked panels or panels not in this output
            if (!panel->isDocked())
                continue;
            if (!panel->view)
                continue;
            if (panel->view->output != (*o).output)
                continue;

            switch (panel->edge()) {
            case WL_HAWAII_PANEL_EDGE_LEFT:
                rect.x += panel->view->surface->width;
                rect.width -= panel->view->surface->width;
                break;
            case WL_HAWAII_PANEL_EDGE_TOP:
                rect.y += panel->view->geometry.y;
                rect.height -= panel->view->surface->height;
                break;
            case WL_HAWAII_PANEL_EDGE_RIGHT:
                rect.width -= panel->view->surface->width;
                break;
            case WL_HAWAII_PANEL_EDGE_BOTTOM:
                rect.height -= panel->view->surface->height;
                break;
            }
        }

        (*o).rect = rect;
    }
}

const struct wl_hawaii_panel_manager_interface DesktopShell::m_panelManagerImpl = {
    wrapInterface(&DesktopShell::setPanelSurface)
};

void DesktopShell::mapNotificationSurfaces()
{
    bool isRight = m_notificationsCornerAnchor & DesktopShell::EdgeRight;
    bool isBottom = m_notificationsCornerAnchor & DesktopShell::EdgeBottom;

    IRect2D availableGeometry = windowsArea(getDefaultOutput());
    int32_t left = availableGeometry.x;
    int32_t top = availableGeometry.y;
    int32_t right = availableGeometry.x + availableGeometry.width;
    int32_t bottom = availableGeometry.y + availableGeometry.height;
    int32_t x, y;

    if (isRight)
        x = right - m_notificationsMargin;
    else
        x = left + m_notificationsMargin;
    if (isBottom)
        y = bottom - m_notificationsMargin;
    else
        y = top + m_notificationsMargin;

    for (weston_view *view: m_notificationsLayer) {
        if (isBottom)
            y -= view->surface->height;
        if (isRight)
            x -= view->surface->width;
        weston_view_set_position(view, x, y);
        if (isRight)
            x += view->surface->width;
        if (isBottom)
            y -= m_notificationsMargin;
        else
            y += view->surface->height + m_notificationsMargin;
    }

    weston_compositor_schedule_repaint(compositor());
}

void DesktopShell::addNotificationSurface(wl_client *client, wl_resource *resource,
                                          wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);
    view->output = getDefaultOutput();

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        DesktopShell *shell = static_cast<DesktopShell *>(es->configure_private);
        shell->mapNotificationSurfaces();
    };
    surface->configure_private = this;
    surface->output = getDefaultOutput();

    if (m_notificationsOrder == DesktopShell::OrderNewestFirst)
        m_notificationsLayer.addSurface(view);
    else
        m_notificationsLayer.prependSurface(view);
    mapNotificationSurfaces();
}

const struct wl_notification_daemon_interface DesktopShell::m_notificationDaemonImpl = {
    wrapInterface(&DesktopShell::addNotificationSurface)
};

int DesktopShell::screenSaverTimeout()
{
    weston_log("screensaver timeout...\n");
    fadeOut();
    return 1;
}

void DesktopShell::screenSaverSigChild(int status)
{
    m_screenSaverChild.process.pid = 0;
    m_screenSaverChild.client = nullptr; // already destroyed by wayland

    if (m_locked)
        weston_compositor_sleep(compositor());
}

void DesktopShell::launchScreenSaverProcess()
{
    if (m_screenSaverBinding)
        return;

    if (m_screenSaverPath.empty() || !m_screenSaverEnabled) {
        weston_compositor_sleep(compositor());
        return;
    }

    if (m_screenSaverChild.process.pid != 0) {
        weston_log("old screensaver still running\n");
        return;
    }

    m_screenSaverChild.client = weston_client_launch(compositor(),
                                                     &m_screenSaverChild.process,
                                                     m_screenSaverPath.c_str(),
                                                     [](struct weston_process *process, int status) {
        ScreenSaverChild *child = container_of(process, ScreenSaverChild, process);
        child->shell->screenSaverSigChild(status);
    });

    if (!m_screenSaverChild.client)
        weston_log("not able to start %s\n", m_screenSaverPath.c_str());
}

void DesktopShell::terminateScreenSaverProcess()
{
    if (m_screenSaverChild.process.pid == 0)
        return;

    ::kill(m_screenSaverChild.process.pid, SIGTERM);
}

void DesktopShell::screenSaverConfigure(weston_surface *es, int32_t sx, int32_t sy)
{
    // Starting screensaver beforehand doesn't work
    if (!m_locked)
        return;

    weston_view *view = container_of(es->views.next, weston_view, surface_link);

    centerSurfaceOnOutput(view, es->output);

    if (wl_list_empty(&view->layer_link)) {
        m_lockLayer.prependSurface(view);
        weston_view_update_transform(view);
        wl_event_source_timer_update(m_screenSaverTimer, m_screenSaverDuration);
        fadeIn();
    }
}

void DesktopShell::setScreenSaverSurface(wl_client *client, wl_resource *resource,
                                         wl_resource *output_resource,
                                         wl_resource *surface_resource)
{
    struct weston_output *output = static_cast<weston_output *>(output_resource->data);
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        static_cast<DesktopShell *>(es->configure_private)->screenSaverConfigure(es, sx, sy); };
    surface->configure_private = this;
    surface->output = output;
}

const struct wl_screensaver_interface DesktopShell::m_screenSaverImpl = {
    wrapInterface(&DesktopShell::setScreenSaverSurface)
};

WL_EXPORT int
module_init(struct weston_compositor *ec, int *argc, char *argv[])
{

    char *client = nullptr;
    if (asprintf(&client, "%s/starthawaii", INSTALL_LIBEXECDIR) == -1) {
        const char *msg = "Can't allocate client executable path";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "STATUS=%s: %s\n"
                   "ERRNO=%i",
                   msg, strerror(errno), errno);
#endif
        weston_log("%s\n", msg);
        return -1;
    }

    DesktopShell *shell = Shell::load<DesktopShell>(ec, client);
    if (!shell) {
        const char *msg = "Cannot create DesktopShell instance";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "STATUS=%s\n"
                   "ERRNO=%i",
                   msg, EFAULT);
#endif
        weston_log("%s\n", msg);
        return -1;
    }

    char *sfile = nullptr;
    if (sfile)
        shell->m_sessionManager = new SessionManager(sfile);

    shell->init();

#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "READY=1\n"
                   "STATUS=Ready\n"
                   "MAINPID=%lu",
                   (unsigned long) getpid());
#endif

    return 0;
}


class DesktopShellSettings : public Settings
{
public:
    DesktopShellSettings()
    {
    }

    ~DesktopShellSettings()
    {
    }

    inline DesktopShell *shell() const
    {
        return static_cast<DesktopShell *>(Shell::instance());
    }

    virtual std::list<Option> options() const override
    {
        std::list<Option> list;
        list.push_back(Option::binding("move_window", Binding::Type::Button));
        list.push_back(Option::binding("resize_window", Binding::Type::Button));
        list.push_back(Option::binding("close_window", Binding::Type::Key));
        list.push_back(Option::binding("previous_workspace", Binding::Type::Key));
        list.push_back(Option::binding("next_workspace", Binding::Type::Key));
        list.push_back(Option::binding("quit", Binding::Type::Key));
        return list;
    }

    virtual void unSet(const std::string &name) override
    {
        if (name == "move_window") {
            shell()->m_moveBinding->reset();
        } else if (name == "resize_window") {
            shell()->m_resizeBinding->reset();
        } else if (name == "close_window") {
            shell()->m_closeBinding->reset();
        } else if (name == "previous_workspace") {
            shell()->m_prevWsBinding->reset();
        } else if (name == "next_workspace") {
            shell()->m_nextWsBinding->reset();
        } else if (name == "quit") {
            shell()->m_quitBinding->reset();
        }
    }

    virtual void set(const std::string &name, const Option::BindingValue &v) override
    {
        if (name == "move_window") {
            v.bind(shell()->m_moveBinding);
        } else if (name == "resize_window") {
            v.bind(shell()->m_resizeBinding);
        } else if (name == "close_window") {
            v.bind(shell()->m_closeBinding);
        } else if (name == "previous_workspace") {
            v.bind(shell()->m_prevWsBinding);
        } else if (name == "next_workspace") {
            v.bind(shell()->m_nextWsBinding);
        } else if (name == "quit") {
            v.bind(shell()->m_quitBinding);
        }
    }
};

SETTINGS(desktop_shell, DesktopShellSettings)
