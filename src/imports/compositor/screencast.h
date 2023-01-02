// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SCREENCAST_H
#define SCREENCAST_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QScreen)

class ScreenCast : public QObject
{
    Q_OBJECT
public:
    explicit ScreenCast(QObject *parent = nullptr);

    Q_INVOKABLE void enable(QScreen *screen);
    Q_INVOKABLE void disable(QScreen *screen);

Q_SIGNALS:
    void frameAvailable(QScreen *screen,
                        const QSize &size, const QPoint &offset,
                        quint32 drmFormat, quint64 modifier,
                        quint32 numObjects);
    void objectAvailable(QScreen *screen,
                         quint32 index, quint32 fd, quint32 size,
                         quint32 offset, quint32 stride,
                         quint32 planeIndex);
    void captureReady(QScreen *screen, quint64 tv_sec, quint32 tv_nsec);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // SCREENCAST_H
