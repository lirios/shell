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
#include "outputconfiguration.h"
#include "outputconfigurationadaptor.h"
#include "outputdevice.h"

OutputConfiguration::OutputConfiguration(QObject *parent)
    : QObject(parent)
    , m_adaptor(new OutputConfigurationAdaptor(this))
{
}

QQmlListProperty<OutputChangeset> OutputConfiguration::changes()
{
    return QQmlListProperty<OutputChangeset>(this, nullptr, changesCount, changesAt);
}

OutputChangeset *OutputConfiguration::pendingChanges(OutputDevice *device)
{
    if (!m_changes.value(device, nullptr))
        m_changes[device] = new OutputChangeset(device, this);
    return m_changes[device];
}

bool OutputConfiguration::hasPendingChanges(OutputDevice *device)
{
    if (!m_changes.value(device, nullptr))
        return false;

    OutputChangeset *changeset = m_changes[device];
    return changeset->isEnabledChanged() ||
            changeset->isCurrentModeIndexChanged() ||
            changeset->isTransformChanged() ||
            changeset->isPositionChanged() ||
            changeset->isScaleFactorChanged();
}

void OutputConfiguration::clearPendingChanges()
{
    qDeleteAll(m_changes.begin(), m_changes.end());
    m_changes.clear();
}

void OutputConfiguration::setApplied()
{
    if (m_done) {
        qWarning("Cannot apply output configuration when it is completed");
        return;
    }

    m_done = true;

    Q_EMIT m_adaptor->Applied();
}

void OutputConfiguration::setFailed()
{
    if (m_done) {
        qWarning("Cannot discard output configuration when it is completed");
        return;
    }

    m_done = true;

    Q_EMIT m_adaptor->Failed();
}

int OutputConfiguration::changesCount(QQmlListProperty<OutputChangeset> *prop)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return that->m_changes.values().count();
}

OutputChangeset *OutputConfiguration::changesAt(QQmlListProperty<OutputChangeset> *prop, int index)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return that->m_changes.values().at(index);
}
