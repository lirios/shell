/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "mixer.h"
#include "mixerbackend.h"

#ifdef HAVE_ALSA
#include "alsamixerbackend.h"
#endif

#ifdef HAVE_PULSEAUDIO
#include "pulseaudiomixerbackend.h"
#endif

Q_LOGGING_CATEGORY(MIXER, "hawaii.qml.mixer")

Mixer::Mixer(QObject *parent)
    : QObject(parent)
    , m_backend(Q_NULLPTR)
{
    // Try to create the PulseAudio backend first, then Alsa
#ifdef HAVE_PULSEAUDIO
    m_backend = PulseAudioMixerBackend::createBackend(this);
#endif
#ifdef HAVE_ALSA
    if (!m_backend)
        m_backend = AlsaMixerBackend::createBackend(this);
#endif

    // Warn if no backend is available
    if (!m_backend)
        qCWarning(MIXER) << "No mixer backend available";
}

Mixer::~Mixer()
{
    delete m_backend;
}

void Mixer::initialize()
{
    // Can't continue if no backend is available
    if (!m_backend)
        return;

    qCDebug(MIXER) << "Using" << m_backend->name() << "mixer backend";

    m_backend->boundaries(&m_min, &m_max);
    m_step = (m_max - m_min) / 50;
}

bool Mixer::isAvailable() const
{
    return m_backend != Q_NULLPTR;
}

QString Mixer::backendName() const
{
    if (!m_backend)
        return QString();
    return m_backend->name();
}

int Mixer::master() const
{
    if (!m_backend)
        return 0;

    return (qreal)m_backend->volume() * (qreal)100 / (qreal)m_max;
}

void Mixer::setMaster(int value)
{
    if (m_backend)
        m_backend->setVolume(qBound(m_min, (int)((qreal)value * (qreal)m_max / (qreal)100), m_max));
}

void Mixer::increaseMaster()
{
    if (m_backend)
        m_backend->setVolume(qBound(m_min, m_backend->volume() + m_step, m_max));
}

void Mixer::decreaseMaster()
{
    if (m_backend)
        m_backend->setVolume(qBound(m_min, m_backend->volume() - m_step, m_max));
}

bool Mixer::isMuted() const
{
    if (m_backend)
        return m_backend->isMuted();
    return false;
}

void Mixer::setMuted(bool value)
{
    if (m_backend)
        m_backend->setMuted(value);
}

#include "moc_mixer.cpp"
