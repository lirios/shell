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

#include <weston/compositor.h>

class Shell;
class ShellSurface;

class Effect {
public:
    Effect(Shell *shell);
    void addSurface(ShellSurface *surf);
    void removeSurface(ShellSurface *surf);

    virtual void run(struct wl_seat *seat, uint32_t time, uint32_t key) {}

protected:
    virtual void addedSurface(ShellSurface *surf) {}
    virtual void removedSurface(ShellSurface *surf) {}

    inline Shell *shell() const { return m_shell; }

private:
    Shell *m_shell;

};

#endif
