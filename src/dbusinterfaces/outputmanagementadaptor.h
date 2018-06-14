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

#ifndef LIRI_OUTPUTMANAGEMENTADAPTOR_H
#define LIRI_OUTPUTMANAGEMENTADAPTOR_H

#include <QDBusObjectPath>
#include <QObject>

class OutputConfiguration;
class OutputManagement;

class OutputManagementAdaptor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QDBusObjectPath> OutputDevices READ outputDevices)
    Q_PROPERTY(QDBusObjectPath PrimaryOutput READ primaryOutput WRITE setPrimaryOutput NOTIFY PrimaryOutputChanged)
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.OutputManagement1")
public:
    explicit OutputManagementAdaptor(OutputManagement *parent);
    ~OutputManagementAdaptor();

    QList<QDBusObjectPath> outputDevices() const;

    QDBusObjectPath primaryOutput() const;
    void setPrimaryOutput(const QDBusObjectPath &handle);

    Q_SCRIPTABLE QDBusObjectPath CreateConfiguration();

Q_SIGNALS:
    void PrimaryOutputChanged(const QDBusObjectPath &outputDevice);
    void OutputDeviceAdded(const QDBusObjectPath &outputDevice);
    void OutputDeviceRemoved(const QDBusObjectPath &outputDevice);

private:
    OutputManagement *m_parent = nullptr;
    QString m_primaryOutput;

    friend class OutputManagement;
};

#endif // LIRI_OUTPUTMANAGEMENTADAPTOR_H
