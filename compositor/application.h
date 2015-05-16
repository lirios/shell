/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore/QObject>

#include <GreenIsland/HomeApplication>

using namespace GreenIsland;

class QLocalSocket;

class Application : public QObject, public HomeApplication
{
    Q_OBJECT
public:
    Application(const QString &sessionSocket = QString());

public Q_SLOTS:
    void lockSession();
    void unlockSession();

protected:
    void compositorLaunched() Q_DECL_OVERRIDE;

private:
    QLocalSocket *m_sessionSocket;

private Q_SLOTS:
    void connected();
    void disconnected();
    void readyRead();
    void error();
};

#endif // APPLICATION_H
