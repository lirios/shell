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

#include "extensions/outputconfiguration_p.h"
#include "extensions/quickoutputconfiguration.h"

QuickOutputConfiguration::QuickOutputConfiguration()
    : OutputConfiguration()
{
}

QQmlListProperty<OutputChangeset> QuickOutputConfiguration::changes()
{
    return QQmlListProperty<OutputChangeset>(this, nullptr, changesCount, changesAt);
}

QQmlListProperty<QObject> QuickOutputConfiguration::data()
{
    return QQmlListProperty<QObject>(this, nullptr, dataAppend, dataCount, dataAt, dataClear);
}

int QuickOutputConfiguration::changesCount(QQmlListProperty<OutputChangeset> *prop)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return OutputConfigurationPrivate::get(that)->changes.values().count();
}

OutputChangeset *QuickOutputConfiguration::changesAt(QQmlListProperty<OutputChangeset> *prop, int index)
{
    OutputConfiguration *that = static_cast<OutputConfiguration *>(prop->object);
    return OutputConfigurationPrivate::get(that)->changes.values().at(index);
}

void QuickOutputConfiguration::dataAppend(QQmlListProperty<QObject> *prop, QObject *object)
{
    QuickOutputConfiguration *that = static_cast<QuickOutputConfiguration *>(prop->object);
    that->m_objects.append(object);
}

int QuickOutputConfiguration::dataCount(QQmlListProperty<QObject> *prop)
{
    QuickOutputConfiguration *that = static_cast<QuickOutputConfiguration *>(prop->object);
    return that->m_objects.count();
}

QObject *QuickOutputConfiguration::dataAt(QQmlListProperty<QObject> *prop, int index)
{
    QuickOutputConfiguration *that = static_cast<QuickOutputConfiguration *>(prop->object);
    return that->m_objects.at(index);
}

void QuickOutputConfiguration::dataClear(QQmlListProperty<QObject> *prop)
{
    QuickOutputConfiguration *that = static_cast<QuickOutputConfiguration *>(prop->object);
    that->m_objects.clear();
}
