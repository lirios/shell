/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "sessionleader.h"

#include <unistd.h>

SessionLeader::SessionLeader(bool sessionLeader, QObject *parent)
    : QProcess(parent)
    , m_sessionLeader(sessionLeader)
{
}

bool SessionLeader::isSessionLeader() const
{
    return m_sessionLeader;
}

void SessionLeader::setSessionLeader(bool value)
{
    m_sessionLeader = value;
}

void SessionLeader::setupChildProcess()
{
    // Set session leader if requested to make logind
    // integration work
    if (m_sessionLeader)
        ::setsid();
}
