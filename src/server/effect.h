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

#ifndef EFFECT_H
#define EFFECT_H

#include <unordered_map>
#include <list>

#include <weston/compositor.h>

#include "settings.h"

class Shell;
class ShellSurface;
class Binding;

class Effect {
public:
    typedef std::unordered_map<std::string, Binding *> Bindings;

    Effect();
    virtual ~Effect();
    void addSurface(ShellSurface *surf);
    void removeSurface(ShellSurface *surf);

    const Bindings &bindings() const { return m_bindings; }
    Binding *binding(const std::string &name);

protected:
    virtual void addedSurface(ShellSurface *surf) {}
    virtual void removedSurface(ShellSurface *surf) {}

    void addBinding(const std::string &n, Binding *b);

    class Settings : public ::Settings
    {
    protected:
        Settings();
        virtual std::list<Option> options() const override;
    };

private:
    Bindings m_bindings;

};

#endif
