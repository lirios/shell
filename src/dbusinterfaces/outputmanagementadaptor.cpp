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

#include <QDBusConnection>

#include "outputconfiguration.h"
#include "outputconfigurationadaptor.h"
#include "outputdevice.h"
#include "outputdeviceadaptor.h"
#include "outputmanagement.h"
#include "outputmanagementadaptor.h"

const QString dbusObjectPath = QLatin1String("/io/liri/Shell/OutputManagement1");

OutputManagementAdaptor::OutputManagementAdaptor(OutputManagement *parent)
    : QObject(parent)
    , m_parent(parent)
{
    QDBusConnection::sessionBus().registerObject(dbusObjectPath, this, QDBusConnection::ExportAllContents);
}

OutputManagementAdaptor::~OutputManagementAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(dbusObjectPath);
}

QList<QDBusObjectPath> OutputManagementAdaptor::outputDevices() const
{
    QList<QDBusObjectPath> list;

    for (auto device : m_parent->devices())
        list.append(device->m_adaptor->path());

    return list;
}

QDBusObjectPath OutputManagementAdaptor::primaryOutput() const
{
    if (!m_parent->primaryOutputDevice())
        return QDBusObjectPath();
    return m_parent->primaryOutputDevice()->m_adaptor->path();
}

void OutputManagementAdaptor::setPrimaryOutput(const QDBusObjectPath &handle)
{
    auto device = m_parent->findChild<OutputDevice *>(handle.path());
    if (device) {
        m_parent->setPrimaryOutputDevice(device);
        Q_EMIT PrimaryOutputChanged(handle);
    }
}

QDBusObjectPath OutputManagementAdaptor::CreateConfiguration()
{
    auto outputConfiguration = new OutputConfiguration(m_parent);
    Q_EMIT m_parent->outputConfigurationCreated(outputConfiguration);
    return outputConfiguration->m_adaptor->path();
}
