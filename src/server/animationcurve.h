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

#ifndef ANIMATIONCURVE_H
#define ANIMATIONCURVE_H

#include <stdio.h>

class AnimationCurve {
public:
    AnimationCurve() {}
    virtual ~AnimationCurve() {}

    virtual float value(float progress) = 0;
};

// These curves are taken from Qt's QEasingCurve.
// See http://qt-project.org/doc/qt-5.0/qtcore/qeasingcurve.html
// and http://qt.gitorious.org/qt/qtbase/blobs/stable/src/3rdparty/easing/easing.cpp

/*
Disclaimer for Robert Penner's Easing Equations license:

TERMS OF USE - EASING EQUATIONS

Open source under the BSD License.

Copyright © 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of contributors may be used
      to endorse or promote products derived from this software without specific
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

class InQuadCurve : public AnimationCurve {
public:
    virtual float value(float f) override {
        return f * f;
    }
};

class InOutQuadCurve : public AnimationCurve {
public:
    virtual float value(float f) override {
        f *= 2.f;
        if (f < 1.f) {
            return f * f/ 2.f;
        } else {
            --f;
            return -0.5f * (f * (f - 2.f) - 1.f);
        }
    }
};

class BackCurve : public AnimationCurve {
public:
    BackCurve() : m_overshoot(1.70158f) {}
    void setOvershoot(float overshoot) { m_overshoot = overshoot; }

protected:
    float m_overshoot;
};


class OutBackCurve : public BackCurve {
public:
    virtual float value(float t) override {
        float s = m_overshoot;
        t -= 1.f;
        return t*t*((s+1)*t+ s) + 1;
    }
};


class InOutBackCurve : public BackCurve {
public:
    virtual float value(float t) override {
        float s = m_overshoot;
        t *= 2.0;
        if (t < 1) {
            s *= 1.525f;
            return 0.5*(t*t*((s+1)*t - s));
        } else {
            t -= 2;
            s *= 1.525f;
            return 0.5*(t*t*((s+1)*t+ s) + 2);
        }
    }
};

class OutBounceCurve : public AnimationCurve {
public:
    virtual float value(float t) override {
        float c = 1.f;
        float a = 0.5f;
        if (t < (4/11.0)) {
            return c*(7.5625*t*t);
        } else if (t < (8/11.0)) {
            t -= (6/11.0);
            return -a * (1. - (7.5625*t*t + .75)) + c;
        } else if (t < (10/11.0)) {
            t -= (9/11.0);
            return -a * (1. - (7.5625*t*t + .9375)) + c;
        } else {
            t -= (21/22.0);
            return -a * (1. - (7.5625*t*t + .984375)) + c;
        }
    }
};

class ElasticCurve : public AnimationCurve {
public:
    ElasticCurve() : m_amplitude(0.2f), m_period(0.7f) {}
    void setAmplitide(float a) { m_amplitude = a; }
    void setPeriod(float p) { m_period = p; }

protected:
    float m_amplitude;
    float m_period;
};

class OutElasticCurve : public ElasticCurve {
public:
    virtual float value(float t) override {
        if (t == 0.f) return 0.f;
        if (t == 1.f) return 1.f;

        float a = m_amplitude;
        float s;
        if(m_amplitude < 1.f) {
            a = 1.f;
            s = m_period / 4.0f;
        } else {
            s = m_period / (2.f * M_PI) * asin(1.f / a);
        }
        return (a * pow(2.0f, -10 * t) * sin((t - s) * (2 * M_PI) / m_period) + 1.f);
    }
};


// This comes instead from http://stereopsis.com/stopping/

class PulseCurve : public AnimationCurve {
public:
    PulseCurve() { m_pulseNormalize = 1.f; m_pulseNormalize = 1.f / pulse(1); }

    // viscous fluid with a pulse for part and decay for the rest
    virtual float value(float x) override {
        if (x >= 1) return 1;
        if (x <= 0) return 0;

        return pulse(x);
    }

private:
    // viscous fluid with a pulse for part and decay for the rest
    float pulse(float x)
    {
        const float pulseScale = 8; // ratio of "tail" to "acceleration"
        float val;

        // test
        x = x * pulseScale;
        if (x < 1) {
            val = x - (1 - exp(-x));
        } else {
            // the previous animation ended here:
            float start = exp(-1);

            // simple viscous drag
            x -= 1;
            float expx = 1 - exp(-x);
            val = start + (expx * (1.0 - start));
        }

        return val * m_pulseNormalize;
    }

    float m_pulseNormalize;
};

#endif
