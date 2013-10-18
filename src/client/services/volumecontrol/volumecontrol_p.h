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

#ifndef VOLUMECONTROL_P_H
#define VOLUMECONTROL_P_H

#include "volumecontrol.h"

#include <alsa/asoundlib.h>

class KeyBinding;

class VolumeControlPrivate
{
    Q_DECLARE_PUBLIC(VolumeControl)
public:
    VolumeControlPrivate();

    VolumeControl *q_ptr;

    snd_mixer_t *mixer;
    snd_mixer_selem_id_t *selemId;
    snd_mixer_elem_t *selem;
    long min, max;

    KeyBinding *upBinding;
    KeyBinding *downBinding;
    KeyBinding *muteBinding;

    void initialize();

    long rawVolume() const;
    void setRawVolume(long value);

    void _q_upTriggered();
    void _q_downTriggered();
    void _q_muteTriggered();
};

#endif // VOLUMECONTROL_P_H
