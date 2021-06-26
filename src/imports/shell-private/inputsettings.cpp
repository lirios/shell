// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QWaylandCompositor>
#include <QWaylandSeat>

#include "inputsettings.h"

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
