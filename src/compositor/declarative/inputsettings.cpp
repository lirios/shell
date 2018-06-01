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

#include <QWaylandCompositor>
#include <QWaylandSeat>

#include "declarative/inputsettings.h"

/*
 * InputSettings
 */

InputSettings::InputSettings(QObject *parent)
    : QObject(parent)
{
}

QWaylandCompositor *InputSettings::compositor() const
{
    return m_compositor;
}

void InputSettings::setCompositor(QWaylandCompositor *compositor)
{
    if (m_compositor == compositor)
        return;

    if (m_compositor)
        disconnect(compositor, SIGNAL(defaultSeatChanged(QWaylandSeat*,QWaylandSeat*)),
                   this, SLOT(setupKeyboard()));

    if (compositor) {
        connect(compositor, SIGNAL(defaultSeatChanged(QWaylandSeat*,QWaylandSeat*)),
                this, SLOT(setupKeyboard()));
        setupKeyboard();
    }

    m_compositor = compositor;
    Q_EMIT compositorChanged();
}

Qt::KeyboardModifier InputSettings::windowActionKey() const
{
    return m_windowActionKey;
}

void InputSettings::setWindowActionKey(Qt::KeyboardModifier mod)
{
    if (m_windowActionKey == mod)
        return;

    m_windowActionKey = mod;
    Q_EMIT windowActionKeyChanged();
}

quint32 InputSettings::keyboardRepeatRate() const
{
    return m_repeatRate;
}

void InputSettings::setKeyboardRepeatRate(quint32 rate)
{
    if (m_repeatRate == rate)
        return;

    m_repeatRate = rate;
    Q_EMIT keyboardRepeatRateChanged();

    if (m_compositor) {
        QWaylandSeat *seat = m_compositor->defaultSeat();
        if (seat && seat->keyboard())
            seat->keyboard()->setRepeatRate(m_repeatRate);
    }
}

quint32 InputSettings::keyboardRepeatDelay() const
{
    return m_repeatDelay;
}

void InputSettings::setKeyboardRepeatDelay(quint32 delay)
{
    if (m_repeatDelay == delay)
        return;

    m_repeatDelay = delay;
    Q_EMIT keyboardRepeatDelayChanged();

    if (m_compositor) {
        QWaylandSeat *seat = m_compositor->defaultSeat();
        if (seat && seat->keyboard())
            seat->keyboard()->setRepeatDelay(m_repeatDelay);
    }
}

void InputSettings::classBegin()
{
}

void InputSettings::componentComplete()
{
    m_initialized = true;
    setupKeyboard();
}

void InputSettings::setupKeyboard()
{
    if (!m_compositor || !m_initialized)
        return;

    QWaylandSeat *seat = m_compositor->defaultSeat();
    if (seat && seat->keyboard()) {
        seat->keyboard()->setRepeatRate(m_repeatRate);
        seat->keyboard()->setRepeatDelay(m_repeatDelay);
    }
}
