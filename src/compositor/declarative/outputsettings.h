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

#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QScreen>

class OutputSettings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QScreen *screen READ screen WRITE setScreen NOTIFY screenChanged)
    Q_PROPERTY(PowerState powerState READ powerState WRITE setPowerState NOTIFY powerStateChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum PowerState {
        PowerStateOn,
        PowerStateStandby,
        PowerStateSuspend,
        PowerStateOff
    };
    Q_ENUM(PowerState)

    explicit OutputSettings(QObject *parent = nullptr);

    QScreen *screen() const;
    void setScreen(QScreen *screen);

    PowerState powerState() const;
    void setPowerState(PowerState state);

Q_SIGNALS:
    void screenChanged();
    void powerStateChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    bool m_initialized = false;
    QScreen *m_screen = nullptr;
    PowerState m_powerState = PowerStateOn;
};
