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

#ifndef DESKTOP_SHELL_H
#define DESKTOP_SHELL_H

#include "shell.h"

class DesktopShell : public Shell
{
public:
    DesktopShell(struct weston_compositor *ec);

protected:
    virtual void init();
    virtual void setGrabCursor(uint32_t);
    virtual void setBusyCursor(ShellSurface *shsurf, struct weston_seat *seat) override;
    virtual void endBusyCursor(struct weston_seat *seat) override;

private:
    static const struct hawaii_desktop_shell_interface m_desktopShellImpl;

    void bind(struct wl_client *client, uint32_t version, uint32_t id);
    void unbind(struct wl_resource *resource);
    void moveBinding(struct wl_seat *seat, uint32_t time, uint32_t button);

    void setAvailableGeometry(struct wl_client *client, struct wl_resource *resource,
                              struct wl_resource *output_resource,
                              int32_t x, int32_t y, int32_t width, int32_t height);

    void setBackground(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                       struct wl_resource *surface_resource);
    void setPanel(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                  struct wl_resource *surface_resource);
    void setPanelGeometry(struct wl_client *client, struct wl_resource *resource,
                          struct wl_resource *output_resource,
                          struct wl_resource *surface_resource,
                          int32_t x, int32_t y, int32_t width, int32_t height);

    void setLauncher(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                     struct wl_resource *surface_resource);
    void setLauncherGeometry(struct wl_client *client, struct wl_resource *resource,
                             struct wl_resource *output_resource,
                             struct wl_resource *surface_resource,
                             int32_t x, int32_t y, int32_t width, int32_t height);

    void setSpecial(struct wl_client *client, struct wl_resource *resource,
                    struct wl_resource *output_resource,
                    struct wl_resource *surface_resource);

    void setPosition(struct wl_client *client, struct wl_resource *resource,
                     struct wl_resource *surface_resource,
                     int32_t x, int32_t y);

    void hideSurface(struct wl_client *client, struct wl_resource *resource,
                     struct wl_resource *surface_resource);

    void setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);
    void unlock(struct wl_client *client, struct wl_resource *resource);
    void setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);

    static void hawaii_desktop_shell_set_available_geometry(struct wl_client *client, struct wl_resource *resource,
                                                            struct wl_resource *output_resource,
                                                            int32_t x, int32_t y, int32_t width, int32_t height);
    static void hawaii_desktop_shell_set_background(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                                    struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_set_panel(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                               struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_set_panel_geometry(struct wl_client *client, struct wl_resource *resource,
                                                        struct wl_resource *output_resource,
                                                        struct wl_resource *surface_resource,
                                                        int32_t x, int32_t y, int32_t width, int32_t height);
    static void hawaii_desktop_shell_set_launcher(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                                  struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_set_launcher_geometry(struct wl_client *client, struct wl_resource *resource,
                                                           struct wl_resource *output_resource,
                                                           struct wl_resource *surface_resource,
                                                           int32_t x, int32_t y, int32_t width, int32_t height);
    static void hawaii_desktop_shell_set_special(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                                 struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_set_position(struct wl_client *client, struct wl_resource *resource,
                                                  struct wl_resource *surface_resource,
                                                  int32_t x, int32_t y);
    static void hawaii_desktop_shell_hide_surface(struct wl_client *client, struct wl_resource *resource,
                                                  struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_set_lock_surface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);
    static void hawaii_desktop_shell_unlock(struct wl_client *client, struct wl_resource *resource);
    static void hawaii_desktop_shell_set_grab_surface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);
};

#define _this static_cast<DesktopShell *>(resource->data)
inline void DesktopShell::hawaii_desktop_shell_set_available_geometry(struct wl_client *client, struct wl_resource *resource,
                                                                      struct wl_resource *output_resource,
                                                                      int32_t x, int32_t y, int32_t width, int32_t height) {
    _this->setAvailableGeometry(client, resource, output_resource, x, y, width, height);
}

inline void DesktopShell::hawaii_desktop_shell_set_background(struct wl_client *client, struct wl_resource *resource,
                                                              struct wl_resource *output_resource, struct wl_resource *surface_resource) {
    _this->setBackground(client, resource, output_resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_panel(struct wl_client *client, struct wl_resource *resource,
                                                         struct wl_resource *output_resource, struct wl_resource *surface_resource) {
    _this->setPanel(client, resource, output_resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_panel_geometry(struct wl_client *client, struct wl_resource *resource,
                                                                  struct wl_resource *output_resource, struct wl_resource *surface_resource,
                                                                  int32_t x, int32_t y, int32_t width, int32_t height) {
    _this->setPanelGeometry(client, resource, output_resource, surface_resource, x, y, width, height);
}

inline void DesktopShell::hawaii_desktop_shell_set_launcher(struct wl_client *client, struct wl_resource *resource,
                                                            struct wl_resource *output_resource, struct wl_resource *surface_resource) {
    _this->setLauncher(client, resource, output_resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_launcher_geometry(struct wl_client *client, struct wl_resource *resource,
                                                                     struct wl_resource *output_resource, struct wl_resource *surface_resource,
                                                                     int32_t x, int32_t y, int32_t width, int32_t height) {
    _this->setLauncherGeometry(client, resource, output_resource, surface_resource, x, y, width, height);
}

inline void DesktopShell::hawaii_desktop_shell_set_special(struct wl_client *client, struct wl_resource *resource,
                                                           struct wl_resource *output_resource, struct wl_resource *surface_resource) {
    _this->setSpecial(client, resource, output_resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_position(struct wl_client *client, struct wl_resource *resource,
                                                            struct wl_resource *surface_resource,
                                                            int32_t x, int32_t y) {
    _this->setPosition(client, resource, surface_resource, x, y);
}

inline void DesktopShell::hawaii_desktop_shell_hide_surface(struct wl_client *client, struct wl_resource *resource,
                                                            struct wl_resource *surface_resource) {
    _this->hideSurface(client, resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_lock_surface(struct wl_client *client, struct wl_resource *resource,
                                                                struct wl_resource *surface_resource) {
    _this->setLockSurface(client, resource, surface_resource);
}

inline void DesktopShell::hawaii_desktop_shell_unlock(struct wl_client *client, struct wl_resource *resource) {
    _this->unlock(client, resource);
}

inline void DesktopShell::hawaii_desktop_shell_set_grab_surface(struct wl_client *client, struct wl_resource *resource,
                                                                struct wl_resource *surface_resource) {
    _this->setGrabSurface(client, resource, surface_resource);
}

#endif
