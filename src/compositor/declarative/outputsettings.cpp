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

#include <qpa/qplatformscreen.h>

#include "logging_p.h"
#include "declarative/outputsettings.h"

OutputSettings::OutputSettings(QObject *parent)
    : QObject(parent)
{
}

QScreen *OutputSettings::screen() const
{
    return m_screen;
}

void OutputSettings::setScreen(QScreen *screen)
{
    if (m_initialized) {
        qCWarning(lcShell) << "WaylandOutputSettings cannot change screen after initialization";
        return;
    }

    if (screen == m_screen)
        return;

    m_screen = screen;
    Q_EMIT screenChanged();
}

OutputSettings::PowerState OutputSettings::powerState() const
{
    return m_powerState;
}

void OutputSettings::setPowerState(OutputSettings::PowerState state)
{
    if (!m_screen) {
        qCWarning(lcShell) << "WaylandOutputSettings cannot set power state if the screen property is not set";
        return;
    }

    if (state == m_powerState)
        return;

    switch (state) {
    case PowerStateOn:
        m_screen->handle()->setPowerState(QPlatformScreen::PowerStateOn);
        break;
    case PowerStateStandby:
        m_screen->handle()->setPowerState(QPlatformScreen::PowerStateStandby);
        break;
    case PowerStateSuspend:
        m_screen->handle()->setPowerState(QPlatformScreen::PowerStateSuspend);
        break;
    case PowerStateOff:
        m_screen->handle()->setPowerState(QPlatformScreen::PowerStateOff);
        break;
    }

    m_powerState = state;
    Q_EMIT powerStateChanged();
}

void OutputSettings::classBegin()
{
}

void OutputSettings::componentComplete()
{
    if (m_screen) {
        switch (m_screen->handle()->powerState()) {
        case QPlatformScreen::PowerStateOn:
            m_powerState = PowerStateOn;
            break;
        case QPlatformScreen::PowerStateStandby:
            m_powerState = PowerStateStandby;
            break;
        case QPlatformScreen::PowerStateSuspend:
            m_powerState = PowerStateSuspend;
            break;
        case QPlatformScreen::PowerStateOff:
            m_powerState = PowerStateOff;
            break;
        }

        Q_EMIT powerStateChanged();
    }

    m_initialized = true;
}
