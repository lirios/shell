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

#include "animation.h"
#include "animationcurve.h"

Animation::Animation()
         : m_curve(nullptr)
{
    m_animation.parent = this;
    wl_list_init(&m_animation.ani.link);
    m_animation.ani.frame = [](struct weston_animation *base, struct weston_output *output, uint32_t msecs) {
        AnimWrapper *animation = container_of(base, AnimWrapper, ani);
        animation->parent->update(output, msecs);
    };
}

Animation::~Animation()
{
    stop();
}

void Animation::setStart(float value)
{
    m_start = value;
}

void Animation::setTarget(float value)
{
    m_target = value;
}

void Animation::run(struct weston_output *output, uint32_t duration, Animation::Flags flags)
{
    stop();

    m_duration = duration;
    m_runFlags = flags;
    m_animation.ani.frame_counter = 0;

    wl_list_insert(&output->animation_list, &m_animation.ani.link);
    weston_compositor_schedule_repaint(output->compositor);
}

void Animation::stop()
{
    if (isRunning()) {
        wl_list_remove(&m_animation.ani.link);
        wl_list_init(&m_animation.ani.link);
    }
}

bool Animation::isRunning() const
{
    //can't use wl_list_empty here because it wants a wl_link* while i have a const wl_link*
    return m_animation.ani.link.next != &m_animation.ani.link;
}

void Animation::update(struct weston_output *output, uint32_t msecs)
{
    if (m_animation.ani.frame_counter <= 1) {
        m_timestamp = msecs;
    }

    uint32_t time = msecs - m_timestamp;
    if (time > m_duration) {
        updateSignal(m_target);
        if ((int)Flags::SendDone & (int)m_runFlags) {
            doneSignal();
        }
        stop();
        weston_compositor_schedule_repaint(output->compositor);
        return;
    }

    float f = (float)time / (float)m_duration;
    if (m_curve) {
        f = m_curve->value(f);
    }
    updateSignal(m_target * f + m_start * (1.f - f));

    weston_compositor_schedule_repaint(output->compositor);
}