/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 * Copyright (C) 2013-2014 Giulio Camuffo
 *
 * Author(s):
 *    Giulio Camuffo <giuliocamuffo@gmail.com>
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "pulseaudiomixerbackend.h"

#include <pulse/glib-mainloop.h>
#include <pulse/volume.h>

Q_LOGGING_CATEGORY(PULSEAUDIO, "hawaii.qml.mixer.pulseaudio")

struct Sink
{
    Sink()
        : muted(false)
    {
    }

    uint32_t index;
    pa_cvolume volume;
    bool muted;
};

PulseAudioMixerBackend::PulseAudioMixerBackend(Mixer *mixer)
    : MixerBackend()
    , m_mixer(mixer)
    , m_loop(Q_NULLPTR)
    , m_loopApi(Q_NULLPTR)
    , m_context(Q_NULLPTR)
    , m_sink(new Sink)
{
}

PulseAudioMixerBackend::~PulseAudioMixerBackend()
{
    delete m_sink;
    cleanup();
}

PulseAudioMixerBackend *PulseAudioMixerBackend::createBackend(Mixer *mixer)
{
    PulseAudioMixerBackend *backend = new PulseAudioMixerBackend(mixer);

    backend->m_loop = pa_glib_mainloop_new(Q_NULLPTR);
    if (!backend->m_loop) {
        qCWarning(PULSEAUDIO) << "Failed to start PulseAudio main loop";
        delete backend;
        return Q_NULLPTR;
    }

    backend->m_loopApi = pa_glib_mainloop_get_api(backend->m_loop);
    if (!backend->m_loopApi) {
        qCWarning(PULSEAUDIO) << "Failed to get PulseAudio main loop API";
        delete backend;
        return Q_NULLPTR;
    }

    backend->m_context = pa_context_new(backend->m_loopApi, Q_NULLPTR);
    if (!backend->m_context) {
        qCWarning(PULSEAUDIO) << "Failed to get PulseAudio context";
        delete backend;
        return Q_NULLPTR;
    }

    pa_context_set_state_callback(backend->m_context, [](pa_context *context, void *data) {
        static_cast<PulseAudioMixerBackend *>(data)->contextStateCallback(context);
    }, backend);
    if (pa_context_connect(backend->m_context, Q_NULLPTR, (pa_context_flags_t)0, Q_NULLPTR) < 0) {
        qCWarning(PULSEAUDIO,
                  "Failed to connect to PulseAudio context: %s",
                  pa_strerror(pa_context_errno(backend->m_context)));
        delete backend;
        return Q_NULLPTR;
    }

    return backend;
}

void PulseAudioMixerBackend::contextStateCallback(pa_context *context)
{
    switch (pa_context_get_state(context)) {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;
    case PA_CONTEXT_READY:
        pa_context_set_subscribe_callback(context, [](pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *data) {
            static_cast<PulseAudioMixerBackend *>(data)->subscribeCallback(c, t, index);
        }, this);
        pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK, Q_NULLPTR, Q_NULLPTR);

        pa_context_get_sink_info_list(context, [](pa_context *c, const pa_sink_info *i, int eol, void *data) {
            static_cast<PulseAudioMixerBackend *>(data)->sinkCallback(c, i, eol);
        }, this);
        break;
    case PA_CONTEXT_TERMINATED:
        cleanup();
        break;
    case PA_CONTEXT_FAILED:
    default:
        qCWarning(PULSEAUDIO,
                  "Connection to the PulseAudio server failed: %s",
                  pa_strerror(pa_context_errno(context)));
        cleanup();
        break;
    }
}

void PulseAudioMixerBackend::subscribeCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t index)
{
    Q_UNUSED(index);

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
    case PA_SUBSCRIPTION_EVENT_SINK:
        pa_context_get_sink_info_list(context, [](pa_context *c, const pa_sink_info *i, int eol, void *data) {
            static_cast<PulseAudioMixerBackend *>(data)->sinkCallback(c, i, eol);
        }, this);
        break;
    default:
        break;
    }
}

void PulseAudioMixerBackend::sinkCallback(pa_context *context, const pa_sink_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(context) == PA_ERR_NOENTITY)
            return;

        qCWarning(PULSEAUDIO) << "Sink callback failure";
        return;
    }

    if (eol > 0)
        return;

    m_sink->index = i->index;
    if (m_sink->muted != (bool)i->mute) {
        m_sink->muted = (bool)i->mute;
        Q_EMIT m_mixer->mutedChanged();
    }
    m_sink->volume = i->volume;
    Q_EMIT m_mixer->masterChanged();
}

void PulseAudioMixerBackend::cleanup()
{
}

QString PulseAudioMixerBackend::name() const
{
    return QStringLiteral("PulseAudio");
}

void PulseAudioMixerBackend::boundaries(int *min, int *max) const
{
    if (min)
        *min = PA_VOLUME_MUTED;
    if (max)
        *max = PA_VOLUME_NORM;
}

int PulseAudioMixerBackend::volume() const
{
    return pa_cvolume_avg(&m_sink->volume);
}

void PulseAudioMixerBackend::setVolume(int value)
{
    setMuted(false);

#if PA_CHECK_VERSION(5, 0, 0)
    bool valid = pa_channels_valid(m_sink->volume.channels) == 1;
#else
    bool valid = true;
#endif
    if (valid) {
        pa_cvolume_set(&m_sink->volume, m_sink->volume.channels, value);
        pa_context_set_sink_volume_by_index(m_context, m_sink->index, &m_sink->volume, Q_NULLPTR, Q_NULLPTR);
    }
}

bool PulseAudioMixerBackend::isMuted() const
{
    return m_sink->muted;
}

void PulseAudioMixerBackend::setMuted(bool value)
{
    pa_context_set_sink_mute_by_index(m_context, m_sink->index, value, Q_NULLPTR, Q_NULLPTR);
}
