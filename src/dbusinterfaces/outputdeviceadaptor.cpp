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
#include <QDBusMetaType>
#include <QtMath>

#include "outputdevice.h"
#include "outputdeviceadaptor.h"

const QString dbusObjectPath = QLatin1String("/io/liri/Shell/OutputDevice1/");

static QString aspectRatioString(const QSize &size)
{
    if (!size.isValid())
        return QString();

    int ratio = 0;
    if (size.width() > size.height())
        ratio = size.width() * 10 / size.height();
    else
        ratio = size.height() * 10 / size.width();

    switch (ratio) {
    case 10:
        return QLatin1String("1:1");
    case 12:
        return QLatin1String("5:4");
    case 13:
        return QLatin1String("4:3");
    case 15:
        return QLatin1String("3:2");
    case 16:
        return QLatin1String("1:6");
    case 17:
        return QLatin1String("16:9");
    case 18:
        return QLatin1String("9:5");
    case 23:
        return QLatin1String("21:9");
    default:
        break;
    }

    return QString();
}

OutputDeviceAdaptor::OutputDeviceAdaptor(const QString &uuid, OutputDevice *parent)
    : QObject(parent)
    , m_parent(parent)
    , m_uuid(uuid)
{
    qDBusRegisterMetaType<Mode>();
    qDBusRegisterMetaType<QList<Mode> >();

    QDBusConnection::sessionBus().registerObject(path().path(), this, QDBusConnection::ExportAllContents);
}

OutputDeviceAdaptor::~OutputDeviceAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(path().path());
}

QDBusObjectPath OutputDeviceAdaptor::path() const
{
    return QDBusObjectPath(dbusObjectPath + m_uuid);
}

QString OutputDeviceAdaptor::uuid() const
{
    return m_uuid;
}

bool OutputDeviceAdaptor::isEnabled() const
{
    return m_enabled;
}

void OutputDeviceAdaptor::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Q_EMIT EnabledChanged(enabled);
    Q_EMIT Changed();
}

QString OutputDeviceAdaptor::manufacturer() const
{
    return m_manufacturer;
}

void OutputDeviceAdaptor::setManufacturer(const QString &manufacturer)
{
    if (m_manufacturer == manufacturer)
        return;

    m_manufacturer = manufacturer;
    Q_EMIT ManufacturerChanged(manufacturer);
    Q_EMIT Changed();
}

QString OutputDeviceAdaptor::model() const
{
    return m_model;
}

void OutputDeviceAdaptor::setModel(const QString &model)
{
    if (m_model == model)
        return;

    m_model = model;
    Q_EMIT ModelChanged(model);
    Q_EMIT Changed();
}

QPoint OutputDeviceAdaptor::position() const
{
    return m_position;
}

void OutputDeviceAdaptor::setPosition(const QPoint &position)
{
    if (m_position == position)
        return;

    m_position = position;
    Q_EMIT PositionChanged(position);
    Q_EMIT Changed();
}

QSize OutputDeviceAdaptor::physicalSize() const
{
    return m_physicalSize;
}

void OutputDeviceAdaptor::setPhysicalSize(const QSize &size)
{
    if (m_physicalSize == size)
        return;

    m_physicalSize = size;
    Q_EMIT PhysicalSizeChanged(size);
    Q_EMIT Changed();
}

QString OutputDeviceAdaptor::transform() const
{
    return m_transform;
}

void OutputDeviceAdaptor::setTransform(const QString &transform)
{
    if (m_transform == transform)
        return;

    m_transform = transform;
    Q_EMIT TransformChanged(transform);
    Q_EMIT Changed();
}

int OutputDeviceAdaptor::scaleFactor() const
{
    return m_scaleFactor;
}

void OutputDeviceAdaptor::setScaleFactor(int scale)
{
    if (m_scaleFactor == scale)
        return;

    m_scaleFactor = scale;
    Q_EMIT ScaleFactorChanged(scale);
    Q_EMIT Changed();
}

QList<Mode> OutputDeviceAdaptor::modes() const
{
    return m_modes;
}

int OutputDeviceAdaptor::currentModeIndex() const
{
    return m_currentModeIndex;
}

void OutputDeviceAdaptor::setCurrentModeIndex(int index)
{
    if (m_currentModeIndex == index)
        return;

    m_currentModeIndex = index;
    Q_EMIT CurrentModeIndexChanged(index);
    Q_EMIT Changed();
}

int OutputDeviceAdaptor::preferredModeIndex() const
{
    return m_preferredModexIndex;
}

void OutputDeviceAdaptor::setPreferredModeIndex(int index)
{
    if (m_preferredModexIndex == index)
        return;

    m_preferredModexIndex = index;
    Q_EMIT PreferredModeIndexChanged(index);
    Q_EMIT Changed();
}

void OutputDeviceAdaptor::addMode(const QSize &size, int refreshRate)
{
    Mode mode;
    mode.name = tr("%1 × %2, %3 Hz (%4)").arg(
                QString::number(size.width()),
                QString::number(size.height()),
                QString::number(refreshRate / 1000),
                aspectRatioString(size));
    mode.size = size;
    mode.refreshRate = refreshRate;
    m_modes.append(mode);

    Q_EMIT ModeAdded(size, refreshRate);
}
