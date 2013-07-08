/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef ANIMATION_H
#define ANIMATION_H

#include <weston/compositor.h>

#include "signal.h"

class ShellSurface;
class AnimationCurve;

class Animation
{
public:
    enum class Flags {
        None = 0,
        SendDone = 1
    };
    Animation();
    ~Animation();

    void setStart(float value);
    void setTarget(float value);
    void run(struct weston_output *output, uint32_t duration, Flags flags = Flags::None);
    void stop();
    bool isRunning() const;
    template<class T>
    void setCurve(const T &curve) { delete m_curve; m_curve = new T; *static_cast<T *>(m_curve) = curve; }

    Signal<float> updateSignal;
    Signal<> doneSignal;

private:
    void update(struct weston_output *output, uint32_t msecs);

    struct AnimWrapper {
        struct weston_animation ani;
        Animation *parent;
    };
    AnimWrapper m_animation;
    float m_start;
    float m_target;
    uint32_t m_duration;
    uint32_t m_timestamp;
    Flags m_runFlags;
    AnimationCurve *m_curve;
};

inline Animation::Flags operator|(Animation::Flags a, Animation::Flags b) {
    return (Animation::Flags)(a | b);
}

#endif // ANIMATION_H
