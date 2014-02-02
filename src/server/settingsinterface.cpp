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

#include "settingsinterface.h"
#include "shell.h"
#include "settings.h"
#include "wayland-settings-server-protocol.h"

SettingsInterface::SettingsInterface()
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &nuclear_settings_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
                         static_cast<SettingsInterface *>(data)->bind(client, version, id);
                     });
}

void SettingsInterface::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &nuclear_settings_interface, version, id);

    if (Shell::instance()->isTrusted(client, "nuclear_settings")) {
        wl_resource_set_implementation(resource, &s_implementation, this, nullptr);

        for (auto &s: SettingsManager::settings()) {
            for (const Option &option: s.second->options()) {
                const std::string &path = s.second->path();
                switch (option.type()) {
                    case Option::Type::String:
                        nuclear_settings_send_string_option(resource, path.c_str(), option.name().c_str());
                        break;
                    case Option::Type::Int:
                        nuclear_settings_send_integer_option(resource, path.c_str(), option.name().c_str());
                        break;
                    case Option::Type::Binding:
                        nuclear_settings_send_binding_option(resource, path.c_str(), option.name().c_str(), (int)option.allowableBindingTypes());
                        break;
                }
            }
        }
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind nuclear_settings denied");
    wl_resource_destroy(resource);
}

void SettingsInterface::unset(wl_client *client, wl_resource *resource, const char *path, const char *name)
{
    SettingsManager::unSet(path, name);
}

void SettingsInterface::setString(wl_client *client, wl_resource *resource, const char *path, const char *name, const char *value)
{
    SettingsManager::set(path, name, value);
}

void SettingsInterface::setInt(wl_client *client, wl_resource *resource, const char *path, const char *name, int32_t value)
{
    SettingsManager::set(path, name, value);
}

void SettingsInterface::setKeyBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t key, uint32_t mod)
{
    SettingsManager::set(path, name, Option::BindingValue::key(key, (weston_keyboard_modifier)mod));
}

void SettingsInterface::setAxisBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t axis, uint32_t mod)
{
    SettingsManager::set(path, name, Option::BindingValue::axis(axis, (weston_keyboard_modifier)mod));
}

void SettingsInterface::setHotSpotBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t hotspot)
{
    SettingsManager::set(path, name, Option::BindingValue::hotSpot((Binding::HotSpot)hotspot));
}

void SettingsInterface::setButtonBinding(wl_client *client, wl_resource *resource, const char *path, const char *name, uint32_t button, uint32_t mod)
{
    SettingsManager::set(path, name, Option::BindingValue::button(button, (weston_keyboard_modifier)mod));
}

const struct nuclear_settings_interface SettingsInterface::s_implementation = {
    wrapInterface(&SettingsInterface::unset),
    wrapInterface(&SettingsInterface::setString),
    wrapInterface(&SettingsInterface::setInt),
    wrapInterface(&SettingsInterface::setKeyBinding),
    wrapInterface(&SettingsInterface::setAxisBinding),
    wrapInterface(&SettingsInterface::setHotSpotBinding),
    wrapInterface(&SettingsInterface::setButtonBinding)
};
