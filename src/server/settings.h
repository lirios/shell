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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <list>
#include <string>
#include <unordered_map>

#include "binding.h"

class Option {
    struct Value;

public:
    enum class Type {
        String,
        Int,
        Binding
    };
    class BindingValue {
    public:
        static BindingValue key(uint32_t key, weston_keyboard_modifier modifier);
        static BindingValue button(uint32_t button, weston_keyboard_modifier modifier);
        static BindingValue axis(uint32_t axis, weston_keyboard_modifier modifier);
        static BindingValue hotSpot(Binding::HotSpot hs);

        void bind(Binding *b) const;

    private:
        BindingValue(Binding::Type t, uint32_t f, uint32_t s);
        BindingValue() : type(0) {}

        void merge(const BindingValue &v);

        int type;
        struct {
            struct {
                uint32_t key;
                weston_keyboard_modifier mod;
            } key;
            struct {
                uint32_t button;
                weston_keyboard_modifier mod;
            } button;
            struct {
                uint32_t axis;
                weston_keyboard_modifier mod;
            } axis;
            Binding::HotSpot hotSpot;
        } value;

        friend struct Value;
        friend class Option;
        friend class SettingsManager;
    };

    static Option string(const char *n);
    static Option integer(const char *n);
    static Option binding(const char *n, Binding::Type allowable);

    Option(const Option &o);

    Option &operator=(const Option &o);

    inline const std::string &name() const { return m_name; }
    inline Type type() const { return m_type; }

    inline bool isSet() const { return m_set; }
    Binding::Type allowableBindingTypes() const;
    std::string valueAsString() const;
    int valueAsInt() const;
    const BindingValue &valueAsBinding() const;

private:
    Option();
    void unSet();

    struct Value {
        Value() {}
        std::string string;
        int integer;
        BindingValue binding;
    };

    bool m_set;
    std::string m_name;
    Type m_type;
    Binding::Type m_allowableBinding;
    Value m_value;

    friend class SettingsManager;
};

class Settings;
class SettingsManager
{
public:
    static bool unSet(const char *path, const char *option);
    static bool set(const char *path, const char *option, const std::string &value);
    static bool set(const char *path, const char *option, int value);
    static bool set(const char *path, const char *option, const Option::BindingValue &value);

    static void init();
    static void cleanup();

    static const std::unordered_map<std::string, Settings *> &settings() { return s_settings; }

private:
    static bool addSettings(Settings *s);

    static std::unordered_map<std::string, Settings *> s_settings;

    friend Settings;
};


class Settings
{
public:
    Settings(const char *group);
    Settings();
    virtual ~Settings() {}

    virtual std::list<Option> options() const = 0;

    virtual void unSet(const std::string &name) {}
    virtual void set(const std::string &name, const std::string &v) {}
    virtual void set(const std::string &name, int v) {}
    virtual void set(const std::string &name, const Option::BindingValue &v) {}

    const Option *option(const std::string &name) const;
    std::string path() const;

    template <class T>
    static bool create(const char *name)
    {
        Settings *s = new T;
        if (!s) {
            return false;
        }

        s->m_name = name;
        SettingsManager::addSettings(s);
        return true;
    }

private:
    std::string m_group;
    std::string m_name;
    std::unordered_map<std::string, Option> m_options;

    friend class SettingsManager;
};

#define SETTINGS(name, class) static const bool __settings_ ## name = Settings::create<class>(#name);

#endif
