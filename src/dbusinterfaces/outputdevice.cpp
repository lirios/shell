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

static QString transformToString(QWaylandOutput::Transform transform)
{
    switch (transform) {
    case QWaylandOutput::TransformNormal:
        return QLatin1String("normal");
    case QWaylandOutput::Transform90:
        return QLatin1String("90");
    case QWaylandOutput::Transform180:
        return QLatin1String("180");
    case QWaylandOutput::Transform270:
        return QLatin1String("270");
    case QWaylandOutput::TransformFlipped:
        return QLatin1String("flipped");
    case QWaylandOutput::TransformFlipped90:
        return QLatin1String("flipped90");
    case QWaylandOutput::TransformFlipped180:
        return QLatin1String("flipped180");
    case QWaylandOutput::TransformFlipped270:
        return QLatin1String("flipped270");
    }

    Q_UNREACHABLE();
}

OutputDevice::OutputDevice(QObject *parent)
    : QObject(parent)
{
}

OutputDevice::~OutputDevice()
{
    if (m_manager)
        m_manager->removeDevice(this);
}

OutputManagement *OutputDevice::manager() const
{
    return m_manager;
}

void OutputDevice::setManager(OutputManagement *manager)
{
    if (m_manager == manager)
        return;

    if (m_initialized) {
        qWarning("Setting OutputDevice::manager after initialization is not possible");
        return;
    }

    m_manager = manager;
    setParent(m_manager);
    Q_EMIT managerChanged();
}

QString OutputDevice::uuid() const
{
    return m_uuid;
}

void OutputDevice::setUuid(const QString &uuid)
{
    if (m_uuid == uuid)
        return;

    if (m_initialized) {
        qWarning("Setting OutputDevice::uuid after initialization is not possible");
        return;
    }

    m_uuid = uuid;
    Q_EMIT uuidChanged();

    m_adaptor = new OutputDeviceAdaptor(m_uuid, this);
}

bool OutputDevice::isEnabled() const
{
    return m_enabled;
}

void OutputDevice::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Q_EMIT enabledChanged();

    if (m_adaptor)
        m_adaptor->setEnabled(enabled);
}

QString OutputDevice::manufacturer() const
{
    return m_manufacturer;
}

void OutputDevice::setManufacturer(const QString &manufacturer)
{
    if (m_manufacturer == manufacturer)
        return;

    m_manufacturer = manufacturer;
    Q_EMIT manufacturerChanged();

    if (m_adaptor)
        m_adaptor->setManufacturer(manufacturer);
}

QString OutputDevice::model() const
{
    return m_model;
}

void OutputDevice::setModel(const QString &model)
{
    if (m_model == model)
        return;

    m_model = model;
    Q_EMIT modelChanged();

    if (m_adaptor)
        m_adaptor->setModel(model);
}

QPoint OutputDevice::position() const
{
    return m_position;
}

void OutputDevice::setPosition(const QPoint &position)
{
    if (m_position == position)
        return;

    m_position = position;
    Q_EMIT positionChanged();

    if (m_adaptor)
        m_adaptor->setPosition(position);
}

QSize OutputDevice::physicalSize() const
{
    return m_physicalSize;
}

void OutputDevice::setPhysicalSize(const QSize &size)
{
    if (m_physicalSize == size)
        return;

    m_physicalSize = size;
    Q_EMIT physicalSizeChanged();

    if (m_adaptor)
        m_adaptor->setPhysicalSize(size);
}

QWaylandOutput::Transform OutputDevice::transform() const
{
    return m_transform;
}

void OutputDevice::setTransform(const QWaylandOutput::Transform &transform)
{
    if (m_transform == transform)
        return;

    m_transform = transform;
    Q_EMIT transformChanged();

    if (m_adaptor)
        m_adaptor->setTransform(transformToString(transform));
}

int OutputDevice::scaleFactor() const
{
    return m_scaleFactor;
}

void OutputDevice::setScaleFactor(int scale)
{
    if (m_scaleFactor == scale)
        return;

    m_scaleFactor = scale;
    Q_EMIT scaleFactorChanged();

    if (m_adaptor)
        m_adaptor->setScaleFactor(scale);
}

int OutputDevice::currentModeIndex() const
{
    return m_currentModeIndex;
}

void OutputDevice::setCurrentModeIndex(int index)
{
    if (m_currentModeIndex == index)
        return;

    m_currentModeIndex = index;
    Q_EMIT currentModeIndexChanged();

    if (m_adaptor)
        m_adaptor->setCurrentModeIndex(index);
}

int OutputDevice::preferredModeIndex() const
{
    return m_preferredModexIndex;
}

void OutputDevice::setPreferredModeIndex(int index)
{
    if (m_initialized) {
        qWarning("Setting OutputDevice::preferredModeIndex after initialization has no effect");
        return;
    }

    if (m_preferredModexIndex == index)
        return;

    m_preferredModexIndex = index;
    Q_EMIT preferredModeIndexChanged();

    if (m_adaptor)
        m_adaptor->setPreferredModeIndex(index);
}

void OutputDevice::addMode(const QSize &size, int refreshRate)
{
    m_modes.append(QWaylandOutputMode(size, refreshRate));
    m_adaptor->addMode(size, refreshRate);
}

void OutputDevice::componentComplete()
{
    m_initialized = true;

    if (m_manager)
        m_manager->addDevice(this);

    Q_ASSERT(m_adaptor);

    m_adaptor->setEnabled(isEnabled());
    m_adaptor->setManufacturer(manufacturer());
    m_adaptor->setModel(model());
    m_adaptor->setPosition(position());
    m_adaptor->setTransform(transformToString(transform()));
    m_adaptor->setScaleFactor(scaleFactor());
    m_adaptor->setCurrentModeIndex(currentModeIndex());
    m_adaptor->setPreferredModeIndex(preferredModeIndex());

    setObjectName(m_adaptor->path().path());
}
