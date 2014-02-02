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

#ifndef SCALEEFFECT_H
#define SCALEEFFECT_H

#include <list>

#include "effect.h"
#include "binding.h"

class ShellGrab;
class Animation;
class Binding;

class ScaleEffect : public Effect
{
public:
    class Settings : public Effect::Settings
    {
    public:
        Settings();
        ~Settings();

        virtual std::list<Option> options() const override;
        virtual void unSet(const std::string &name) override;
        virtual void set(const std::string &name, int v) override;
        virtual void set(const std::string &name, const Option::BindingValue &v) override;

    private:
        ScaleEffect *m_effect;
    };

    ScaleEffect();
    ~ScaleEffect();

protected:
    virtual void addedSurface(ShellSurface *surf);
    virtual void removedSurface(ShellSurface *surf);

private:
    void run(struct weston_seat *ws);
    void run(struct weston_seat *seat, uint32_t time, uint32_t key);
    void run(weston_seat *seat, uint32_t time, Binding::HotSpot hs);
    void end(ShellSurface *surface);

    bool m_scaled;
    std::list<struct SurfaceTransform *> m_surfaces;
    struct weston_seat *m_seat;
    struct Grab *m_grab;
    ShellSurface *m_chosenSurface;

    friend Grab;
};

#endif
