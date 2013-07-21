/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POWERMANAGERBACKEND_H
#define POWERMANAGERBACKEND_H

#include <QtCore/QtGlobal>

enum PowerCapability {
    None = 0x00,
    PowerOff = 0x01,
    Restart = 0x02,
    Suspend = 0x04,
    Hibernate = 0x08,
    HybridSleep = 0x10
};

Q_DECLARE_FLAGS(PowerCapabilities, PowerCapability)
Q_DECLARE_OPERATORS_FOR_FLAGS(PowerCapabilities)

class PowerManagerBackend
{
public:
    explicit PowerManagerBackend();
    virtual ~PowerManagerBackend();

    virtual QString name() const = 0;

    virtual PowerCapabilities capabilities() const = 0;

    virtual void powerOff() = 0;
    virtual void restart() = 0;
    virtual void suspend() = 0;
    virtual void hibernate() = 0;
    virtual void hybridSleep() = 0;
};

#endif // POWERMANAGERBACKEND_H
