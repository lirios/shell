/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QVariant>

#include "mixersource.h"

#include <alsa/asoundlib.h>

/*
 * MixerSourcePrivate
 */

class MixerSourcePrivate
{
public:
    MixerSourcePrivate();

    void initialize();

    long rawVolume() const;
    void setRawVolume(long value);

    int volume() const;
    void setVolume(int value);

    void increaseVolume();
    void decreaseVolume();

    bool isMute() const;
    void setMute(bool value);

    snd_mixer_t *mixer;
    snd_mixer_selem_id_t *selemId;
    snd_mixer_elem_t *selem;
    long min, max;
};

MixerSourcePrivate::MixerSourcePrivate()
    : mixer(0)
    , selemId(0)
    , selem(0)
    , min(0)
    , max(0)
{
    // Initialize
    initialize();
}

void MixerSourcePrivate::initialize()
{
    do {
        // Open mixer
        if (snd_mixer_open(&mixer, 0) < 0)
            break;
        snd_mixer_attach(mixer, "default");
        snd_mixer_selem_register(mixer, 0, 0);
        snd_mixer_load(mixer);

        // Master element
        snd_mixer_selem_id_alloca(&selemId);
        snd_mixer_selem_id_set_index(selemId, 0);
        snd_mixer_selem_id_set_name(selemId, "Master");
        selem = snd_mixer_find_selem(mixer, selemId);
        if (!selem)
            break;
        snd_mixer_selem_get_playback_volume_range(selem, &min, &max);

        return;
    } while (0);

    mixer = 0;
    selemId = 0;
    selem = 0;
}

long MixerSourcePrivate::rawVolume() const
{
    if (!selem)
        return 0;

    long vol;
    snd_mixer_selem_get_playback_volume(selem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
    return vol;
}

void MixerSourcePrivate::setRawVolume(long value)
{
    if (!selem)
        return;

    // Volume must be in range
    if (value < min)
        value = min;
    if (value > max)
        value = max;

    snd_mixer_selem_set_playback_volume(selem, SND_MIXER_SCHN_FRONT_LEFT, value);
    snd_mixer_selem_set_playback_volume(selem, SND_MIXER_SCHN_FRONT_RIGHT, value);
}

int MixerSourcePrivate::volume() const
{
    long vol = rawVolume();
    int v = 100.f * vol / max;
    return v;
}

void MixerSourcePrivate::setVolume(int value)
{
    long vol = value * max / 100;
    setRawVolume(vol);
}

void MixerSourcePrivate::increaseVolume()
{
    setVolume(volume() + 5);
}

void MixerSourcePrivate::decreaseVolume()
{
    setVolume(volume() - 5);
}

bool MixerSourcePrivate::isMute() const
{
    if (!selem)
        return true;

    int mute = 0;
    snd_mixer_selem_get_playback_switch(selem, SND_MIXER_SCHN_FRONT_LEFT, &mute);
    return !mute;
}

void MixerSourcePrivate::setMute(bool value)
{
    if (!selem)
        return;

    if (isMute() != value)
        snd_mixer_selem_set_playback_switch_all(selem, !value);
}

/*
 * MixerSource
 */

MixerSource::MixerSource(QObject *parent)
    : Hawaii::DataSource(parent)
    , d_ptr(new MixerSourcePrivate())
{
    setObjectName("Master");
}

MixerSource::~MixerSource()
{
    delete d_ptr;
}

void MixerSource::setMute(bool value)
{
    Q_D(MixerSource);
    d->setMute(value);
}

void MixerSource::setVolume(int value)
{
    Q_D(MixerSource);
    d->setVolume(value);
}

void MixerSource::checkForUpdate()
{
    Q_D(MixerSource);

    setEntry("mute", d->isMute());
    setEntry("volume", d->volume());
    update();
}

#include "moc_mixersource.cpp"
