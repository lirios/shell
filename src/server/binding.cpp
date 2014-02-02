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

#include "binding.h"
#include "shell.h"

Binding *Binding::s_toggledBinding = nullptr;

Binding::Binding()
        : m_binding(nullptr)
        , m_isToggle(false)
        , m_type(0)
{
}

Binding::~Binding()
{
    if (m_binding) {
        weston_binding_destroy(m_binding);
    }
    if (m_type & (int)Type::HotSpot) {
        Shell::instance()->removeHotSpotBinding(this);
    }
}

void Binding::reset()
{
    if (m_binding) {
        weston_binding_destroy(m_binding);
        m_binding = nullptr;
    }
    if (m_type & (int)Type::HotSpot) {
        Shell::instance()->removeHotSpotBinding(this);
    }
    m_type = 0;
}

void Binding::setIsToggle(bool t)
{
    m_isToggle = t;
}

void Binding::releaseToggle()
{
    if (m_isToggle && s_toggledBinding == this) {
        s_toggledBinding = nullptr;
    }
}

void Binding::keyHandler(weston_seat *seat, uint32_t time, uint32_t key, void *data)
{
    Binding *b = static_cast<Binding *>(data);
    if (b->checkToggled()) {
        b->keyTriggered(seat, time, key);
    }
}

void Binding::buttonHandler(weston_seat *seat, uint32_t time, uint32_t button, void *data)
{
    Binding *b = static_cast<Binding *>(data);
    if (b->checkToggled()) {
        b->buttonTriggered(seat, time, button);
    }
}

static void axisHandler(weston_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value, void *data)
{
    static_cast<Binding *>(data)->axisTriggered(seat, time, axis, value);
}

void Binding::bindKey(uint32_t key, weston_keyboard_modifier modifier)
{
    if (m_binding && m_type & (int)Type::Key) {
        weston_binding_destroy(m_binding);
    }
    m_binding = weston_compositor_add_key_binding(Shell::instance()->compositor(), key, modifier, keyHandler, this);
    m_type |= (int)Type::Key;
}

void Binding::bindButton(uint32_t button, weston_keyboard_modifier modifier)
{
    if (m_binding && m_type & (int)Type::Button) {
        weston_binding_destroy(m_binding);
    }
    m_binding = weston_compositor_add_button_binding(Shell::instance()->compositor(), button, modifier, buttonHandler, this);
    m_type |= (int)Type::Button;
}

void Binding::bindAxis(uint32_t axis, weston_keyboard_modifier modifier)
{
    if (m_binding && m_type & (int)Type::Axis) {
        weston_binding_destroy(m_binding);
    }
    m_binding = weston_compositor_add_axis_binding(Shell::instance()->compositor(), axis, modifier, axisHandler, this);
    m_type |= (int)Type::Axis;
}

void Binding::bindHotSpot(HotSpot hs)
{
    if (m_type & (int)Type::HotSpot) {
        Shell::instance()->removeHotSpotBinding(this);
    }
    Shell::instance()->bindHotSpot(hs, this);
    m_type |= (int)Type::HotSpot;
}

void Binding::hotSpotHandler(weston_seat *seat, uint32_t time, HotSpot hs)
{
    if (checkToggled()) {
        hotSpotTriggered(seat, time, hs);
    }
}

bool Binding::checkToggled()
{
    if (!m_isToggle) {
        return true;
    }

    if (s_toggledBinding && s_toggledBinding != this) {
        return false;
    }
    if (s_toggledBinding == this) {
        s_toggledBinding = nullptr;
    } else {
        s_toggledBinding = this;
    }
    return true;
}
