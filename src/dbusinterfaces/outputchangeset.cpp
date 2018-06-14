/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "outputchangeset.h"
#include "outputdevice.h"

OutputChangeset::OutputChangeset(OutputDevice *outputDevice, QObject *parent)
    : QObject(parent)
    , m_outputDevice(outputDevice)
{
}

bool OutputChangeset::isEmpty() const
{
    return !isEnabledChanged() &&
            !isTransformChanged() &&
            !isCurrentModeIndexChanged() &&
            !isPositionChanged() &&
            !isScaleFactorChanged();
}

OutputDevice *OutputChangeset::outputDevice() const
{
    return m_outputDevice;
}

bool OutputChangeset::isEnabledChanged() const
{
    return m_enabled != m_outputDevice->isEnabled();
}

bool OutputChangeset::isTransformChanged() const
{
    return m_transform != m_outputDevice->transform();
}

bool OutputChangeset::isCurrentModeIndexChanged() const
{
    return m_currentModeIndex != m_outputDevice->currentModeIndex();
}

bool OutputChangeset::isPositionChanged() const
{
    return m_position != m_outputDevice->position();
}

bool OutputChangeset::isScaleFactorChanged() const
{
    return m_scaleFactor != m_outputDevice->scaleFactor();
}

bool OutputChangeset::isEnabled() const
{
    return m_enabled;
}

void OutputChangeset::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

int OutputChangeset::currentModeIndex() const
{
    return m_currentModeIndex;
}

void OutputChangeset::setCurrentModeIndex(int index)
{
    m_currentModeIndex = index;
}

QWaylandOutput::Transform OutputChangeset::transform() const
{
    return m_transform;
}

void OutputChangeset::setTransform(QWaylandOutput::Transform transform)
{
    m_transform = transform;
}

QPoint OutputChangeset::position() const
{
    return m_position;
}

void OutputChangeset::setPosition(const QPoint &position)
{
    m_position = position;
}

int OutputChangeset::scaleFactor() const
{
    return m_scaleFactor;
}

void OutputChangeset::setScaleFactor(int scaleFactor)
{
    m_scaleFactor = scaleFactor;
}
