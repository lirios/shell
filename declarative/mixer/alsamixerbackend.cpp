/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "alsamixerbackend.h"

Q_LOGGING_CATEGORY(ALSA, "hawaii.qml.mixer.alsa")

AlsaMixerBackend::AlsaMixerBackend(Mixer *mixer)
    : MixerBackend()
    , m_mixer(mixer)
    , m_alsaMixer(Q_NULLPTR)
    , m_selemId(Q_NULLPTR)
    , m_selem(Q_NULLPTR)
    , m_min(0)
    , m_max(0)
{
}

AlsaMixerBackend::~AlsaMixerBackend()
{
    if (m_alsaMixer)
        snd_mixer_close(m_alsaMixer);
}

AlsaMixerBackend *AlsaMixerBackend::createBackend(Mixer *mixer)
{
    AlsaMixerBackend *backend = new AlsaMixerBackend(mixer);
    if (!backend)
        return Q_NULLPTR;

    // Open mixer
    if (snd_mixer_open(&backend->m_alsaMixer, 0) < 0) {
        qCWarning(ALSA) << "Failed to open Alsa mixer";
        delete backend;
        return Q_NULLPTR;
    }
    snd_mixer_attach(backend->m_alsaMixer, "default");
    snd_mixer_selem_register(backend->m_alsaMixer, 0, 0);
    snd_mixer_load(backend->m_alsaMixer);

    // Master element
    snd_mixer_selem_id_alloca(&backend->m_selemId);
    snd_mixer_selem_id_set_index(backend->m_selemId, 0);
    snd_mixer_selem_id_set_name(backend->m_selemId, "Master");
    backend->m_selem = snd_mixer_find_selem(backend->m_alsaMixer,
                                            backend->m_selemId);
    if (!backend->m_selem) {
        qCWarning(ALSA) << "Failed to find Master element";
        delete backend;
        return Q_NULLPTR;
    }
    snd_mixer_selem_get_playback_volume_range(backend->m_selem,
                                              &backend->m_min,
                                              &backend->m_max);

    return backend;
}

QString AlsaMixerBackend::name() const
{
    return QStringLiteral("Alsa");
}

void AlsaMixerBackend::boundaries(int *min, int *max) const
{
    if (min)
        *min = m_min;
    if (max)
        *max = m_max;
}

int AlsaMixerBackend::volume() const
{
    long vol = 0;
    snd_mixer_selem_get_playback_volume(m_selem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
    return vol;
}

void AlsaMixerBackend::setVolume(int value)
{
    // Volume must be in range
    if (value < m_min) {
        qCWarning(ALSA) << "Volume" << value << "too low, assume" << m_min;
        value = m_min;
    }
    if (value > m_max) {
        qCWarning(ALSA) << "Volume" << value << "too high, assume" << m_max;
        value = m_max;
    }

    snd_mixer_selem_set_playback_volume(m_selem, SND_MIXER_SCHN_FRONT_LEFT, value);
    snd_mixer_selem_set_playback_volume(m_selem, SND_MIXER_SCHN_FRONT_RIGHT, value);
    Q_EMIT m_mixer->masterChanged();
}

bool AlsaMixerBackend::isMuted() const
{
    int mute = 0;
    snd_mixer_selem_get_playback_switch(m_selem, SND_MIXER_SCHN_FRONT_LEFT, &mute);
    return !mute;
}

void AlsaMixerBackend::setMuted(bool value)
{
    snd_mixer_selem_set_playback_switch_all(m_selem, !value);
    Q_EMIT m_mixer->mutedChanged();
}
