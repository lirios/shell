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

#ifndef MINIMIZEEFFECT_H
#define MINIMIZEEFFECT_H

#include <list>

#include "effect.h"

class MinimizeEffect : public Effect
{
public:
    class Settings : public Effect::Settings
    {
    public:
        Settings();
        ~Settings();

        virtual void unSet(const std::string &name) override;
        virtual void set(const std::string &name, int v) override;

    private:
        MinimizeEffect *m_effect;
    };

    MinimizeEffect();
    ~MinimizeEffect();

protected:
    virtual void addedSurface(ShellSurface *surf) override;
    virtual void removedSurface(ShellSurface *surf) override;

private:
    struct Surface;
    std::list<Surface *> m_surfaces;
};

#endif
