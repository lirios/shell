/****************************************************************************
* This file is part of Vibe.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusServiceWatcher>

#include "mpris2engine.h"
#include "mpris2player.h"

const QString mprisInterface("org.mpris.MediaPlayer2");
const QString mprisPlayerInterface("org.mpris.MediaPlayer2.Player");
const QString mprisPrefix("org.mpris.MediaPlayer2.");

Q_LOGGING_CATEGORY(MPRIS2, "vibe.mpris2")

Mpris2Engine::Mpris2Engine(QObject *parent)
    : QObject(parent)
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    const QStringList services = bus.interface()->registeredServiceNames();

    for (const QString &name: services.filter(mprisInterface)) {
        qCDebug(MPRIS2) << "Found player" << name;
        m_players.append(new Mpris2Player(name));
        Q_EMIT playersChanged();
    }

    m_watcher = new QDBusServiceWatcher(this);
    connect(m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, [=](const QString &name, const QString &oldOwner, const QString &newOwner) {
        if (oldOwner.isEmpty() && name.startsWith(mprisPrefix)) {
            qCDebug(MPRIS2) << "Found new player" << name;
            m_players.append(new Mpris2Player(name));
        } else if (newOwner.isEmpty() && name.startsWith(mprisPrefix)) {
            for (int i = 0; i < m_players.size(); i++) {
                if (m_players.at(i)->serviceName() == name) {
                    qCDebug(MPRIS2) << "Remove player" << name;
                    m_players.takeAt(i)->deleteLater();
                    Q_EMIT playersChanged();
                    break;
                }
            }
        }
    });
}

Mpris2Engine::~Mpris2Engine()
{
    while (!m_players.isEmpty())
        m_players.takeFirst()->deleteLater();
}

QQmlListProperty<Mpris2Player> Mpris2Engine::players()
{
    return QQmlListProperty<Mpris2Player>(this, Q_NULLPTR, playersCount, playersAt);
}

int Mpris2Engine::playersCount(QQmlListProperty<Mpris2Player> *prop)
{
    Mpris2Engine *engine = static_cast<Mpris2Engine *>(prop->object);
    return engine->m_players.count();
}

Mpris2Player *Mpris2Engine::playersAt(QQmlListProperty<Mpris2Player> *prop, int index)
{
    Mpris2Engine *engine = static_cast<Mpris2Engine *>(prop->object);
    return engine->m_players.at(index);
}

#include "moc_mpris2engine.cpp"
