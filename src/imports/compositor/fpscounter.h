// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <QElapsedTimer>
#include <QPointer>
#include <QQuickItem>
#include <QQuickWindow>

class FpsCounter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal framesPerSecond READ framesPerSecond NOTIFY framesPerSecondChanged)
public:
    explicit FpsCounter(QQuickItem *parent = nullptr);

    qreal framesPerSecond() const;

Q_SIGNALS:
    void framesPerSecondChanged();

protected:
    void timerEvent(QTimerEvent *) override;

private:
    QPointer<QQuickWindow> m_lastWindow;
    QMetaObject::Connection m_lastWindowConnection;
    QElapsedTimer m_timer;
    int m_counter = 0;
    qreal m_fps = 0;

    void resetFramesPerSecond();

private Q_SLOTS:
    void handleWindowChanged(QQuickWindow *window);
};

#endif // FPSCOUNTER_H
