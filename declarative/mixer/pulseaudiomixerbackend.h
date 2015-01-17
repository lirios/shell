/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013-2014 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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

#ifndef PULSEAUDIOMIXERBACKEND_H
#define PULSEAUDIOMIXERBACKEND_H

#include "mixer.h"
#include "mixerbackend.h"

#include <pulse/pulseaudio.h>

Q_DECLARE_LOGGING_CATEGORY(PULSEAUDIO)

struct pa_glib_mainloop;

struct Sink;

class PulseAudioMixerBackend : public MixerBackend
{
public:
    ~PulseAudioMixerBackend();

    static PulseAudioMixerBackend *createBackend(Mixer *mixer);

    QString name() const;

    void boundaries(int *min, int *max) const;

    int volume() const;
    void setVolume(int value);

    bool isMuted() const;
    void setMuted(bool value);

private:
    Mixer *m_mixer;
    pa_glib_mainloop *m_loop;
    pa_mainloop_api *m_loopApi;
    pa_context *m_context;
    Sink *m_sink;

    PulseAudioMixerBackend(Mixer *mixer);

    void contextStateCallback(pa_context *context);
    void subscribeCallback(pa_context *context,
                           pa_subscription_event_type_t t, uint32_t index);
    void sinkCallback(pa_context *context, const pa_sink_info *i, int eol);
    void cleanup();
};

#endif // PULSEAUDIOMIXERBACKEND_H
