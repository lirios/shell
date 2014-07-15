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

#ifndef BINDING_H
#define BINDING_H

#include <weston/compositor.h>

#include "shellsignal.h"

struct weston_seat;

class Binding {
public:
    enum class Type {
        Key = 1,
        Button = 2,
        Axis = 4,
        HotSpot = 8
    };
    enum class HotSpot {
        TopLeftCorner = 1,
        TopRightCorner = 2,
        BottomLeftCorner = 4,
        BottomRightCorner = 8
    };
    Binding();
    ~Binding();
    void reset();

    void setIsToggle(bool toggle);
    void releaseToggle();

    void bindKey(uint32_t key, weston_keyboard_modifier modifier);
    void bindButton(uint32_t key, weston_keyboard_modifier modifier);
    void bindAxis(uint32_t axis, weston_keyboard_modifier modifier);
    void bindHotSpot(HotSpot hs);

    Signal<weston_seat *, uint32_t, uint32_t> keyTriggered;
    Signal<weston_seat *, uint32_t, uint32_t> buttonTriggered;
    Signal<weston_seat *, uint32_t, uint32_t, wl_fixed_t> axisTriggered;
    Signal<weston_seat *, uint32_t, HotSpot> hotSpotTriggered;

private:
    static void keyHandler(weston_seat *seat, uint32_t time, uint32_t key, void *data);
    static void buttonHandler(weston_seat *seat, uint32_t time, uint32_t key, void *data);
    static Binding *s_toggledBinding;

    void hotSpotHandler(weston_seat *seat, uint32_t time, HotSpot hs);
    bool checkToggled();

    weston_binding *m_binding;
    bool m_isToggle;
    int m_type;

    friend class Shell;
};

inline Binding::Type operator|(Binding::Type a, Binding::Type b)
{
    return (Binding::Type)((int)a | (int)b);
}

inline int operator&(Binding::Type a, Binding::Type b)
{
    return (int)a & (int)b;
}

#endif
