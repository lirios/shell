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

void VolumeControlPrivate::upTriggered()
{
    Q_Q(VolumeControl);
    q->setVolume(q->volume() + 5);
}

void VolumeControlPrivate::downTriggered()
{
    Q_Q(VolumeControl);
    q->setVolume(q->volume() - 5);
}

void VolumeControlPrivate::muteTriggered()
{
    Q_Q(VolumeControl);
    q->setVolume(0);
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

    connect(d->upBinding, SIGNAL(triggered()), this, SLOT(upTriggered()));
    connect(d->downBinding, SIGNAL(triggered()), this, SLOT(downTriggered()));
    connect(d->muteBinding, SIGNAL(triggered()), this, SLOT(muteTriggered()));
}

VolumeControl::~VolumeControl()
{
    delete d_ptr;
}

int VolumeControl::volume() const
{
    Q_D(const VolumeControl);

    long value;
    snd_mixer_selem_get_playback_volume(d->selem, SND_MIXER_SCHN_FRONT_LEFT, &value);
    value *= 100.f / (float)d->max;
    return value;
}

void VolumeControl::setVolume(int value)
{
    // Volume is in [0:100] range
    if (value < 0)
        value = 0;
    if (value > 100)
        value = 100;

    Q_D(VolumeControl);
    snd_mixer_selem_set_playback_volume_all(d->selem, value * d->max / 100.f);

    Q_EMIT volumeChanged(value);
}

#include "moc_volumecontrol.cpp"
