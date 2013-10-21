/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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

#ifndef POLICYKITAGENT_P_H
#define POLICYKITAGENT_P_H

#include <polkitqt1-agent-session.h>
#include <polkitqt1-details.h>
#include <polkitqt1-identity.h>

class PolicyKitAgent;

class PolicyKitAgentPrivate
{
public:
    PolicyKitAgentPrivate(PolicyKitAgent *self);

    bool progressing;
    bool canceled;
    QString realName;
    PolkitQt1::UnixUserIdentity identity;
    QString cookie;
    PolkitQt1::Agent::Session *session;

private:
    Q_DECLARE_PUBLIC(PolicyKitAgent)
    PolicyKitAgent *const q_ptr;
};

#endif // POLICYKITAGENT_P_H
