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

#ifndef LIRI_OUTPUTCONFIGURATION_H
#define LIRI_OUTPUTCONFIGURATION_H

#include <QHash>
#include <QObject>
#include <QQmlListProperty>

class OutputChangeset;
class OutputConfigurationAdaptor;
class OutputDevice;
class OutputManagementAdaptor;

class OutputConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OutputChangeset> changes READ changes)
public:
    explicit OutputConfiguration(QObject *parent = nullptr);

    QQmlListProperty<OutputChangeset> changes();

    OutputChangeset *pendingChanges(OutputDevice *device);
    bool hasPendingChanges(OutputDevice *device);
    void clearPendingChanges();

    Q_INVOKABLE void setApplied();
    Q_INVOKABLE void setFailed();

    static int changesCount(QQmlListProperty<OutputChangeset> *prop);
    static OutputChangeset *changesAt(QQmlListProperty<OutputChangeset> *prop, int index);

Q_SIGNALS:
    void changeRequested();

private:
    OutputConfigurationAdaptor *m_adaptor = nullptr;
    bool m_done = false;
    QHash<OutputDevice *, OutputChangeset *> m_changes;

    friend class OutputManagementAdaptor;
};

#endif // LIRI_OUTPUTCONFIGURATION_H
