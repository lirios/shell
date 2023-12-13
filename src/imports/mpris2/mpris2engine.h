// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_MPRIS2ENGINE_H
#define LIRI_MPRIS2ENGINE_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>
#include <QtQml/QQmlListProperty>

class QDBusServiceWatcher;
class Mpris2Player;

Q_DECLARE_LOGGING_CATEGORY(MPRIS2)

class Mpris2Engine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Mpris2Player> players READ players NOTIFY playersChanged)
public:
    Mpris2Engine(QObject *parent = 0);
    ~Mpris2Engine();

    QQmlListProperty<Mpris2Player> players();
    static qsizetype playersCount(QQmlListProperty<Mpris2Player> *prop);
    static Mpris2Player *playersAt(QQmlListProperty<Mpris2Player> *prop, qsizetype index);

Q_SIGNALS:
    void playersChanged();

private:
    QDBusServiceWatcher *m_watcher;
    QList<Mpris2Player *> m_players;
};

#endif // LIRI_MPRIS2ENGINE_H
