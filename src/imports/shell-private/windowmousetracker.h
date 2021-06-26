// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WINDOWMOUSETRACKER_H
#define WINDOWMOUSETRACKER_H

#include <QtQuick/QQuickItem>

class WindowMouseTrackerPrivate;

class WindowMouseTracker : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mouseXChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mouseYChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(bool windowSystemCursorEnabled READ windowSystemCursorEnabled WRITE setWindowSystemCursorEnabled NOTIFY windowSystemCursorEnabledChanged)
    Q_DECLARE_PRIVATE(WindowMouseTracker)
public:
    explicit WindowMouseTracker(QQuickItem *parent = nullptr);
    ~WindowMouseTracker();

    qreal mouseX() const;
    qreal mouseY() const;

    bool containsMouse() const;

    bool windowSystemCursorEnabled() const;
    void setWindowSystemCursorEnabled(bool enable);

Q_SIGNALS:
    void mouseXChanged();
    void mouseYChanged();
    void containsMouseChanged();
    void windowSystemCursorEnabledChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    WindowMouseTrackerPrivate *const d_ptr;
};

#endif // WINDOWMOUSETRACKER_H
