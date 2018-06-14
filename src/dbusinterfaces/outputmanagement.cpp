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

#include "outputdevice.h"
#include "outputdeviceadaptor.h"
#include "outputmanagement.h"
#include "outputmanagementadaptor.h"

OutputManagement::OutputManagement(QObject *parent)
    : QObject(parent)
    , m_adaptor(new OutputManagementAdaptor(this))
{
}

QVector<OutputDevice *> OutputManagement::devices() const
{
    return m_devices;
}

OutputDevice *OutputManagement::primaryOutputDevice() const
{
    return m_primaryOutputDevice;
}

void OutputManagement::setPrimaryOutputDevice(OutputDevice *device)
{
    if (m_primaryOutputDevice == device)
        return;

    m_primaryOutputDevice = device;
    Q_EMIT primaryOutputDeviceChanged(device);
}

void OutputManagement::addDevice(OutputDevice *device)
{
    m_devices.append(device);
    Q_EMIT outputDeviceAdded(device);
    Q_EMIT m_adaptor->OutputDeviceAdded(device->m_adaptor->path());
}

void OutputManagement::removeDevice(OutputDevice *device)
{
    if (m_devices.removeOne(device)) {
        Q_EMIT outputDeviceRemoved(device);
        Q_EMIT m_adaptor->OutputDeviceRemoved(device->m_adaptor->path());
    }
}
