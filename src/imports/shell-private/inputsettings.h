/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef INPUTSETTINGS_H
#define INPUTSETTINGS_H

#include <QObject>
#include <QQmlParserStatus>

class QWaylandCompositor;

class InputSettings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_PROPERTY(quint32 keyboardRepeatRate READ keyboardRepeatRate WRITE setKeyboardRepeatRate NOTIFY keyboardRepeatRateChanged)
    Q_PROPERTY(quint32 keyboardRepeatDelay READ keyboardRepeatDelay WRITE setKeyboardRepeatDelay NOTIFY keyboardRepeatDelayChanged)
public:
    explicit InputSettings(QObject *parent = nullptr);

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

    quint32 keyboardRepeatRate() const;
    void setKeyboardRepeatRate(quint32 rate);

    quint32 keyboardRepeatDelay() const;
    void setKeyboardRepeatDelay(quint32 delay);

Q_SIGNALS:
    void compositorChanged();
    void keyboardRepeatRateChanged();
    void keyboardRepeatDelayChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    bool m_initialized = false;
    QWaylandCompositor *m_compositor = nullptr;
    quint32 m_repeatRate = 40;
    quint32 m_repeatDelay = 400;

private Q_SLOTS:
    void setupKeyboard();
};

#endif // INPUTSETTINGS_H
