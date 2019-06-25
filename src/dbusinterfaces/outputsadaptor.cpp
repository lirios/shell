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

#include "outputsadaptor.h"

const QString outputsDbusObjectPath = QStringLiteral("/io/liri/Shell/Outputs");
const QString outputDbusObjectPath = QStringLiteral("/io/liri/Shell/Outputs/");

/*
 * OutputAdaptor
 */

OutputAdaptor::OutputAdaptor(const QString &uuid, QObject *parent)
    : QObject(parent)
    , m_path(QDBusObjectPath(outputDbusObjectPath + uuid))
    , m_uuid(uuid)
{
    QDBusConnection::sessionBus().registerObject(
                m_path.path(), this, QDBusConnection::ExportAllContents);
}

OutputAdaptor::~OutputAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(m_path.path());
}

QDBusObjectPath OutputAdaptor::path() const
{
    return m_path;
}

QString OutputAdaptor::uuid() const
{
    return m_uuid;
}

QString OutputAdaptor::manufacturer() const
{
    return m_manufacturer;
}

void OutputAdaptor::setManufacturer(const QString &manufacturer)
{
    if (m_manufacturer == manufacturer)
        return;

    m_manufacturer = manufacturer;
    Q_EMIT ManufacturerChanged(manufacturer);
}

QString OutputAdaptor::model() const
{
    return m_model;
}

void OutputAdaptor::setModel(const QString &model)
{
    if (m_model == model)
        return;

    m_model = model;
    Q_EMIT ModelChanged(model);
}

/*
 * OutputsAdaptor
 */

OutputsAdaptor::OutputsAdaptor(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().registerObject(
                outputsDbusObjectPath, this,
                QDBusConnection::ExportAllContents);
}

OutputsAdaptor::~OutputsAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(outputsDbusObjectPath);
}

QDBusObjectPath OutputsAdaptor::primaryOutput() const
{
    return m_primaryOutputPath;
}

void OutputsAdaptor::setPrimaryOutput(const QString &uuid)
{
    for (auto outputAdaptor : qAsConst(m_outputs)) {
        if (outputAdaptor->uuid() == uuid) {
            // Don't set it twice
            if (m_primaryOutputPath == outputAdaptor->path())
                return;

            m_primaryOutputPath = outputAdaptor->path();
            Q_EMIT PrimaryOutputChanged(m_primaryOutputPath);
            return;
        }
    }
}

QList<QDBusObjectPath> OutputsAdaptor::outputs() const
{
    QList<QDBusObjectPath> list;

    for (auto outputAdaptor : qAsConst(m_outputs))
        list.append(outputAdaptor->path());

    return list;
}

void OutputsAdaptor::addOutput(OutputAdaptor *outputAdaptor)
{
    m_outputs.append(outputAdaptor);
    Q_EMIT OutputAdded(outputAdaptor->path());
}

void OutputsAdaptor::removeOutput(const QString &uuid)
{
    m_outputs.erase(
    std::remove_if(m_outputs.begin(), m_outputs.end(),
                   [this, uuid](OutputAdaptor *outputAdaptor) {
        if (outputAdaptor->uuid() == uuid) {
            Q_EMIT OutputRemoved(outputAdaptor->path());
            return true;
        }

        return false;
    }), m_outputs.end());
}

bool OutputsAdaptor::hasOutput(const QString &uuid)
{
    for (auto outputAdaptor : qAsConst(m_outputs)) {
        if (outputAdaptor->uuid() == uuid)
            return true;
    }

    return false;
}
