// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <Solid/StorageAccess>

#include "storagedevice.h"

StorageDevice::StorageDevice(const QString &udi, QObject *parent)
    : QObject(parent)
    , m_device(Solid::Device(udi))
    , m_ignored(false)
{
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
