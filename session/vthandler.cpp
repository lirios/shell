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

#include "vthandler.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/vt.h>

#ifndef KDSKBMUTE
#define KDSKBMUTE 0x4B51
#endif

#ifdef K_OFF
#define KBD_OFF_MODE K_OFF
#else
#define KBD_OFF_MODE K_RAW
#endif

VtHandler::VtHandler(QObject *parent)
    : QObject(parent)
    , m_active(false)
{
    // Disable vt keyboard so that input doesn't bleed through
    if (::isatty(0)) {
        ::ioctl(0, KDGKBMODE, &m_oldKbdMode);
        ::ioctl(0, KDSKBMUTE, 1);
        ::ioctl(0, KDSKBMODE, KBD_OFF_MODE);
    }
}

VtHandler::~VtHandler()
{
    // Restore vt keyboard mode
    ::ioctl(0, KDSKBMUTE, 0);
    ::ioctl(0, KDSKBMODE, m_oldKbdMode);
}

bool VtHandler::isActive() const
{
    return m_active;
}

void VtHandler::activate(int vt)
{
    ::ioctl(0, VT_ACTIVATE, vt);
    setActive(false);
}

void VtHandler::setActive(bool value)
{
    if (m_active == value)
        return;

    m_active = value;
    Q_EMIT activeChanged(value);
}

#include "moc_vthandler.cpp"
