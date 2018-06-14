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

#ifndef LIRI_OUTPUTMANAGEMENT_H
#define LIRI_OUTPUTMANAGEMENT_H

#include <QObject>

#include "outputdevice.h"

class OutputConfiguration;
class OutputManagementAdaptor;

class OutputManagement : public QObject
{
    Q_OBJECT
    Q_PROPERTY(OutputDevice *primaryOutputDevice READ primaryOutputDevice NOTIFY primaryOutputDeviceChanged)
public:
    explicit OutputManagement(QObject *parent = nullptr);

    QVector<OutputDevice *> devices() const;

    OutputDevice *primaryOutputDevice() const;
    void setPrimaryOutputDevice(OutputDevice *device);

Q_SIGNALS:
    void primaryOutputDeviceChanged(OutputDevice *device);
    void outputDeviceAdded(OutputDevice *device);
    void outputDeviceRemoved(OutputDevice *device);
    void outputConfigurationCreated(OutputConfiguration *configuration);

private:
    OutputManagementAdaptor *m_adaptor = nullptr;
    QVector<OutputDevice *> m_devices;
    OutputDevice *m_primaryOutputDevice = nullptr;

    void addDevice(OutputDevice *device);
    void removeDevice(OutputDevice *device);

    friend class OutputDevice;
};

#endif // LIRI_OUTPUTMANAGEMENT_H
