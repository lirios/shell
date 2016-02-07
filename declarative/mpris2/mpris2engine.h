/****************************************************************************
* This file is part of Hawaii.
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

#ifndef MPRIS2ENGINE_H
#define MPRIS2ENGINE_H

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
    static int playersCount(QQmlListProperty<Mpris2Player> *prop);
    static Mpris2Player *playersAt(QQmlListProperty<Mpris2Player> *prop, int index);

Q_SIGNALS:
    void playersChanged();

private:
    QDBusServiceWatcher *m_watcher;
    QList<Mpris2Player *> m_players;
};

#endif // MPRIS2ENGINE_H
