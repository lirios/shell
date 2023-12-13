// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusServiceWatcher>

#include "mpris2engine.h"
#include "mpris2player.h"

const QString mprisInterface(QStringLiteral("org.mpris.MediaPlayer2"));
const QString mprisPlayerInterface(QStringLiteral("org.mpris.MediaPlayer2.Player"));
const QString mprisPrefix(QStringLiteral("org.mpris.MediaPlayer2."));

Q_LOGGING_CATEGORY(MPRIS2, "liri.mpris2")

Mpris2Engine::Mpris2Engine(QObject *parent)
    : QObject(parent)
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    const auto registeredServices = bus.interface()->registeredServiceNames();

    if (registeredServices.isValid()) {
        const auto services = registeredServices.value().filter(mprisInterface);
        for (const auto &name : services) {
            qCDebug(MPRIS2) << "Found player" << name;
            m_players.append(new Mpris2Player(name));
            Q_EMIT playersChanged();
        }
    }

    m_watcher = new QDBusServiceWatcher(this);
    connect(m_watcher, &QDBusServiceWatcher::serviceOwnerChanged, [this](const QString &name, const QString &oldOwner, const QString &newOwner) {
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
    return QQmlListProperty<Mpris2Player>(this, nullptr, playersCount, playersAt);
}

qsizetype Mpris2Engine::playersCount(QQmlListProperty<Mpris2Player> *prop)
{
    Mpris2Engine *engine = static_cast<Mpris2Engine *>(prop->object);
    return engine->m_players.count();
}

Mpris2Player *Mpris2Engine::playersAt(QQmlListProperty<Mpris2Player> *prop, qsizetype index)
{
    Mpris2Engine *engine = static_cast<Mpris2Engine *>(prop->object);
    return engine->m_players.at(index);
}

#include "moc_mpris2engine.cpp"
