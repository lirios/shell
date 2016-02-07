/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include <Solid/StorageAccess>

#include "storagedevice.h"

Q_LOGGING_CATEGORY(DEVICE, "hawaii.qml.hardware.storagedevice")

StorageDevice::StorageDevice(const QString &udi, QObject *parent)
    : QObject(parent)
    , m_device(Solid::Device(udi))
    , m_ignored(false)
{
    qCDebug(DEVICE) << "Added storage device" << udi;

    Solid::StorageAccess *access = m_device.as<Solid::StorageAccess>();
    m_ignored = access->isIgnored();
    m_filePath = access->filePath();

    connect(access, &Solid::StorageAccess::setupDone,
            [this](Solid::ErrorType error, const QVariant &errorData, const QString &udi) {
        Q_UNUSED(error);
        Q_UNUSED(errorData);
        Q_UNUSED(udi);
        Q_EMIT mountedChanged();
    });
    connect(access, &Solid::StorageAccess::teardownDone,
            [this](Solid::ErrorType error, const QVariant &errorData, const QString &udi) {
        Q_UNUSED(error);
        Q_UNUSED(errorData);
        Q_UNUSED(udi);
        Q_EMIT mountedChanged();
    });
}

StorageDevice::~StorageDevice()
{
    qCDebug(DEVICE) << "Removed storage device" << m_device.udi();
}

QString StorageDevice::udi() const
{
    return m_device.udi();
}

QString StorageDevice::name() const
{
    return m_device.description();
}

QString StorageDevice::iconName() const
{
    return m_device.icon();
}

QString StorageDevice::filePath() const
{
    return m_filePath;
}

bool StorageDevice::isMounted() const
{
    const Solid::StorageAccess *access = m_device.as<Solid::StorageAccess>();
    return access->isAccessible();
}

bool StorageDevice::isIgnored() const
{
    return m_ignored;
}

void StorageDevice::mount()
{
    Solid::StorageAccess *access = m_device.as<Solid::StorageAccess>();
    access->setup();
}

void StorageDevice::unmount()
{
    Solid::StorageAccess *access = m_device.as<Solid::StorageAccess>();
    access->teardown();
}

#include "moc_storagedevice.cpp"
