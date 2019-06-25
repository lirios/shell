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
#include <QWaylandCompositor>
#include <QScreen>
#include <QWindow>

#include "screencast.h"
#include "screencastadaptor.h"

const QString dbusPath = QStringLiteral("/ScreenCast");

ScreenCastAdaptor::ScreenCastAdaptor(QObject *parent)
    : QObject(parent)
    , m_parent(qobject_cast<ScreenCast *>(parent))
{
    connect(m_parent, &ScreenCast::streamReady, this, &ScreenCastAdaptor::streamReady);
    connect(m_parent, &ScreenCast::startStreaming, this, &ScreenCastAdaptor::startStreaming);
    connect(m_parent, &ScreenCast::stopStreaming, this, &ScreenCastAdaptor::stopStreaming);

    auto bus = QDBusConnection::sessionBus();
    bus.registerObject(dbusPath, this, QDBusConnection::ExportAllContents);
}

ScreenCastAdaptor::~ScreenCastAdaptor()
{
    QDBusConnection::sessionBus().unregisterObject(dbusPath);
}

bool ScreenCastAdaptor::captureScreen(const QDBusObjectPath &handle)
{
    QString uuid = handle.path().replace(QStringLiteral("/io/liri/Shell/Outputs/"), QString());

    for (auto output : m_parent->compositor()->outputs()) {
        if (output->property("uuid").toString() == uuid)
            return m_parent->createStream(output->window()->screen());
    }

    qCWarning(lcScreenCast) << "No output with uuid" << uuid << "found";

    return false;
}
