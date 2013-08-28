/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDebug>
#include <QtCore/QtMath>
#include <linux/input.h>

#include "volumecontrol.h"
#include "volumecontrol_p.h"
#include "keybinding.h"
#include "servicefactory.h"
#include "desktopshell.h"

/*
 * VolumeControlPrivate
 */

VolumeControlPrivate::VolumeControlPrivate()
    : min(0)
    , max(0)
{
    // Open mixer
    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, 0, 0);
    snd_mixer_load(mixer);

    // Master element
    snd_mixer_selem_id_alloca(&selemId);
    snd_mixer_selem_id_set_index(selemId, 0);
    snd_mixer_selem_id_set_name(selemId, "Master");
    selem = snd_mixer_find_selem(mixer, selemId);
    snd_mixer_selem_get_playback_volume_range(selem, &min, &max);

    // Bind volume keys
    upBinding = DesktopShell::instance()->addKeyBinding(KEY_VOLUMEUP, 0);
    downBinding = DesktopShell::instance()->addKeyBinding(KEY_VOLUMEDOWN, 0);
    muteBinding = DesktopShell::instance()->addKeyBinding(KEY_MUTE, 0);
}

long VolumeControlPrivate::rawVolume() const
{
    long vol;
    snd_mixer_selem_get_playback_volume(selem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
    return vol;
}

void VolumeControlPrivate::setRawVolume(long value)
{
    // Volume must be in range
    if (value < min)
        value = min;
    if (value > max)
        value = max;

    snd_mixer_selem_set_playback_volume(selem, SND_MIXER_SCHN_FRONT_LEFT, value);
    snd_mixer_selem_set_playback_volume(selem, SND_MIXER_SCHN_FRONT_RIGHT, value);
}

void VolumeControlPrivate::_q_upTriggered()
{
    Q_Q(VolumeControl);

    q->setVolume(q->volume() + 5);
}

void VolumeControlPrivate::_q_downTriggered()
{
    Q_Q(VolumeControl);

    q->setVolume(q->volume() - 5);
}

void VolumeControlPrivate::_q_muteTriggered()
{
    Q_Q(VolumeControl);

    q->setMute(!q->isMute());
}

/*
 * VolumeControl
 */

VolumeControl::VolumeControl(QObject *parent)
    : QObject(parent)
    , d_ptr(new VolumeControlPrivate())
{
    Q_D(VolumeControl);
    d->q_ptr = this;

    connect(d->upBinding, SIGNAL(triggered()),
            this, SLOT(_q_upTriggered()));
    connect(d->downBinding, SIGNAL(triggered()),
            this, SLOT(_q_downTriggered()));
    connect(d->muteBinding, SIGNAL(triggered()),
            this, SLOT(_q_muteTriggered()));
}

VolumeControl::~VolumeControl()
{
    delete d_ptr;
}

bool VolumeControl::isMute() const
{
    Q_D(const VolumeControl);

    int mute = 0;
    snd_mixer_selem_get_playback_switch(d->selem, SND_MIXER_SCHN_FRONT_LEFT, &mute);
    return !mute;
}

void VolumeControl::setMute(bool value)
{
    Q_D(VolumeControl);

    if (isMute() != value) {
        snd_mixer_selem_set_playback_switch_all(d->selem, !value);
        Q_EMIT muteChanged(value);
    }
}

int VolumeControl::volume() const
{
    Q_D(const VolumeControl);
    long vol = d->rawVolume();
    int v = 100.f * vol / d->max;
    return v;
}

void VolumeControl::setVolume(int value)
{
    Q_D(VolumeControl);
    long vol = value * d->max / 100;
    d->setRawVolume(vol);
    Q_EMIT volumeChanged(value);
}

#include "moc_volumecontrol.cpp"
