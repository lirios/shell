// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
