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

#ifndef LIRI_QUICKOUTPUTCONFIGURATION_H
#define LIRI_QUICKOUTPUTCONFIGURATION_H

#include <QQmlListProperty>

#include "outputconfiguration.h"

class QuickOutputConfigurationPrivate;

class QuickOutputConfiguration : public OutputConfiguration
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OutputChangeset> changes READ changes NOTIFY changesChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    QuickOutputConfiguration();

    QQmlListProperty<OutputChangeset> changes();
    QQmlListProperty<QObject> data();

    static int changesCount(QQmlListProperty<OutputChangeset> *prop);
    static OutputChangeset *changesAt(QQmlListProperty<OutputChangeset> *prop, int index);

    static void dataAppend(QQmlListProperty<QObject> *prop, QObject *object);
    static int dataCount(QQmlListProperty<QObject> *prop);
    static QObject *dataAt(QQmlListProperty<QObject> *prop, int index);
    static void dataClear(QQmlListProperty<QObject> *prop);

Q_SIGNALS:
    void changesChanged();

private:
    QVector<QObject *> m_objects;
};

#endif // LIRI_QUICKOUTPUTCONFIGURATION_H
