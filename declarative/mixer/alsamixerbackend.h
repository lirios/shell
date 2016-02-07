/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
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

#ifndef ALSAMIXERBACKEND_H
#define ALSAMIXERBACKEND_H

#include "mixer.h"
#include "mixerbackend.h"

#include <alsa/asoundlib.h>

Q_DECLARE_LOGGING_CATEGORY(ALSA)

class AlsaMixerBackend : public MixerBackend
{
public:
    ~AlsaMixerBackend();

    static AlsaMixerBackend *createBackend(Mixer *mixer);

    QString name() const;

    void boundaries(int *min, int *max) const;

    int volume() const;
    void setVolume(int value);

    bool isMuted() const;
    void setMuted(bool value);

private:
    Mixer *m_mixer;
    snd_mixer_t *m_alsaMixer;
    snd_mixer_selem_id_t *m_selemId;
    snd_mixer_elem_t *m_selem;
    long m_min, m_max;

    AlsaMixerBackend(Mixer *mixer);
};

#endif // ALSAMIXERBACKEND_H
