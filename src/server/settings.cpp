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

#include <assert.h>

#include "settings.h"

Option::BindingValue::BindingValue(Binding::Type t, uint32_t f, uint32_t s)
                    : type((int)t)
{
    switch (t) {
        case Binding::Type::Key:
            value.key.key = f;
            value.key.mod = (weston_keyboard_modifier)s;
        break;
        case Binding::Type::Button:
            value.button.button = f;
            value.button.mod = (weston_keyboard_modifier)s;
            break;
        case Binding::Type::Axis:
            value.axis.axis = f;
            value.axis.mod = (weston_keyboard_modifier)s;
        break;
        case Binding::Type::HotSpot:
            value.hotSpot = (Binding::HotSpot)f;
        break;
    }
}

Option::BindingValue Option::BindingValue::key(uint32_t key, weston_keyboard_modifier modifier)
{
    return BindingValue(Binding::Type::Key, key, modifier);
}

Option::BindingValue Option::BindingValue::button(uint32_t button, weston_keyboard_modifier modifier)
{
    return BindingValue(Binding::Type::Button, button, modifier);
}

Option::BindingValue Option::BindingValue::axis(uint32_t axis, weston_keyboard_modifier modifier)
{
    return BindingValue(Binding::Type::Axis, axis, modifier);
}

Option::BindingValue Option::BindingValue::hotSpot(Binding::HotSpot hs)
{
    return BindingValue(Binding::Type::HotSpot, (uint32_t)hs, 0);
}

void Option::BindingValue::merge(const BindingValue &v)
{
    type |= v.type;
    if (v.type & (int)Binding::Type::Key) {
        value.key = v.value.key;
    }
    if (v.type & (int)Binding::Type::Button) {
        value.button = v.value.button;
    }
    if (v.type & (int)Binding::Type::Axis) {
        value.axis = v.value.axis;
    }
    if (v.type & (int)Binding::Type::HotSpot) {
        value.hotSpot = v.value.hotSpot;
    }
}

void Option::BindingValue::bind(Binding *b) const
{
    b->reset();
    if (type & (int)Binding::Type::Key) {
        b->bindKey(value.key.key, value.key.mod);
    }
    if (type & (int)Binding::Type::Button) {
        b->bindButton(value.button.button, value.button.mod);
    }
    if (type & (int)Binding::Type::Axis) {
        b->bindAxis(value.axis.axis, value.axis.mod);
    }
    if (type & (int)Binding::Type::HotSpot) {
        b->bindHotSpot(value.hotSpot);
    }
}


Option Option::string(const char *n)
{
    Option o;
    o.m_name = n;
    o.m_type = Type::String;
    return o;
}

Option Option::integer(const char *n)
{
    Option o;
    o.m_name = n;
    o.m_type = Type::Int;
    return o;
}

Option Option::binding(const char *n, Binding::Type allowable)
{
    Option o;
    o.m_name = n;
    o.m_type = Type::Binding;
    o.m_allowableBinding = allowable;
    return o;
}

Option::Option()
      : m_set(false)
{
}

Option::Option(const Option &o)
{
    *this = o;
}

Option &Option::operator=(const Option &o)
{
    m_name = o.m_name;
    m_type = o.m_type;
    m_allowableBinding = o.m_allowableBinding;
    m_value = o.m_value;
    return *this;
}

void Option::unSet()
{
    m_set = false;
    switch (m_type) {
        case Type::String: m_value.string = ""; break;
        case Type::Int: m_value.integer = 0; break;
        case Type::Binding: m_value.binding.type = 0; break;
    }
}

Binding::Type Option::allowableBindingTypes() const
{
    if (m_type != Type::Binding) {
        return (Binding::Type)0;
    }

    return m_allowableBinding;
}

std::string Option::valueAsString() const
{
    assert(m_type == Type::String);
    return m_value.string;
}

int Option::valueAsInt() const
{
    assert(m_type == Type::Int);
    return m_value.integer;
}

const Option::BindingValue &Option::valueAsBinding() const
{
    assert(m_type == Type::Binding);
    return m_value.binding;
}




Settings::Settings(const char *group)
        : m_group(group)
{
}

Settings::Settings()
{
}

const Option *Settings::option(const std::string &name) const
{
    auto it = m_options.find(name);
    if (it == m_options.end()) {
        return nullptr;
    }

    return &it->second;
}

std::string Settings::path() const
{
    if (m_group != "") {
        return m_group + "/" + m_name;
    }
    return m_name;
}



std::unordered_map<std::string, Settings *> SettingsManager::s_settings;

bool SettingsManager::addSettings(Settings *s)
{
    auto opts = s->options();
    for (const Option &o: opts) {
        s->m_options.insert(std::pair<std::string, Option>(o.m_name, o));
    }

    s_settings[s->path()] = s;
    return true;
}

bool SettingsManager::unSet(const char *path, const char *option)
{
    Settings *s = s_settings[path];
    if (s) {
        auto it = s->m_options.find(option);
        if (it != s->m_options.end()) {
            it->second.unSet();
            s->unSet(option);
            return true;
        }
    }
    return false;
}

bool SettingsManager::set(const char *path, const char *option, const std::string &v)
{
    Settings *s = s_settings[path];
    if (s) {
        auto it = s->m_options.find(option);
        if (it != s->m_options.end() && it->second.m_type == Option::Type::String) {
            it->second.m_value.string = v;
            it->second.m_set = true;
            s->set(option, v);
            return true;
        }
    }
    return false;
}

bool SettingsManager::set(const char *path, const char *option, int v)
{
    Settings *s = s_settings[path];
    if (s) {
        auto it = s->m_options.find(option);
        if (it != s->m_options.end() && it->second.m_type == Option::Type::Int) {
            it->second.m_value.integer = v;
            it->second.m_set = true;
            s->set(option, v);
            return true;
        }
    }
    return false;
}

bool SettingsManager::set(const char *path, const char *option, const Option::BindingValue &v)
{
    Settings *s = s_settings[path];
    if (s) {
        auto it = s->m_options.find(option);
        if (it != s->m_options.end() && it->second.m_type == Option::Type::Binding && (int)it->second.m_allowableBinding & v.type) {
            it->second.m_value.binding.merge(v);
            it->second.m_set = true;
            s->set(option, it->second.m_value.binding);
            return true;
        }
    }
    return false;
}

void SettingsManager::init()
{
}

void SettingsManager::cleanup()
{
    for (auto &s: s_settings) {
        delete s.second;
    }
}
