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

#include "outputchangeset.h"
#include "outputconfiguration.h"
#include "outputconfigurationadaptor.h"
#include "outputdevice.h"

const QString dbusObjectPath = QStringLiteral("/io/liri/Shell/OutputConfiguration1/");

static QWaylandOutput::Transform stringToTransform(const QString &transform)
{
    if (transform == QLatin1String("normal"))
        return QWaylandOutput::TransformNormal;
    else if (transform == QLatin1String("90"))
        return QWaylandOutput::Transform90;
    else if (transform == QLatin1String("180"))
        return QWaylandOutput::Transform180;
    else if (transform == QLatin1String("270"))
        return QWaylandOutput::Transform270;
    else if (transform == QLatin1String("flipped"))
        return QWaylandOutput::TransformFlipped;
    else if (transform == QLatin1String("flipped90"))
        return QWaylandOutput::TransformFlipped90;
    else if (transform == QLatin1String("flipped180"))
        return QWaylandOutput::TransformFlipped180;
    else if (transform == QLatin1String("flipped270"))
        return QWaylandOutput::TransformFlipped270;

    Q_UNREACHABLE();
}

OutputConfigurationAdaptor::OutputConfigurationAdaptor(OutputConfiguration *parent)
    : QObject(parent)
    , m_parent(parent)
{
    static int id = 1;
    m_id = id++;

    QDBusConnection::sessionBus().registerObject(path().path(), this, QDBusConnection::ExportAllContents);
}

OutputConfigurationAdaptor::~OutputConfigurationAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(path().path());
}

QDBusObjectPath OutputConfigurationAdaptor::path() const
{
    return QDBusObjectPath(dbusObjectPath + QString::number(m_id));
}

void OutputConfigurationAdaptor::SetEnabled(const QDBusObjectPath &handle, bool enabled)
{
    auto device = m_parent->parent()->findChild<OutputDevice *>(handle.path());
    if (device)
        m_parent->pendingChanges(device)->setEnabled(enabled);
}

void OutputConfigurationAdaptor::SetMode(const QDBusObjectPath &handle, int index)
{
    auto device = m_parent->parent()->findChild<OutputDevice *>(handle.path());
    if (device)
        m_parent->pendingChanges(device)->setCurrentModeIndex(index);
}

void OutputConfigurationAdaptor::SetTransform(const QDBusObjectPath &handle, const QString &transform)
{
    auto device = m_parent->parent()->findChild<OutputDevice *>(handle.path());
    if (device)
        m_parent->pendingChanges(device)->setTransform(stringToTransform(transform));
}

void OutputConfigurationAdaptor::SetPosition(const QDBusObjectPath &handle, const QPoint &position)
{
    auto device = m_parent->parent()->findChild<OutputDevice *>(handle.path());
    if (device)
        m_parent->pendingChanges(device)->setPosition(position);
}

void OutputConfigurationAdaptor::SetScaleFactor(const QDBusObjectPath &handle, int scaleFactor)
{
    auto device = m_parent->parent()->findChild<OutputDevice *>(handle.path());
    if (device)
        m_parent->pendingChanges(device)->setScaleFactor(scaleFactor);
}

void OutputConfigurationAdaptor::Apply()
{
    Q_EMIT m_parent->changeRequested();
}
