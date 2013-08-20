/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

#ifndef WINDOW_H
#define WINDOW_H

#include <QtCore/QObject>

class WaylandIntegration;
class WindowPrivate;

class Window : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString identifier READ identifier NOTIFY identifierChanged)
    Q_PROPERTY(States state READ state WRITE setState NOTIFY stateChanged)
    Q_DISABLE_COPY(Window)
    Q_DECLARE_PRIVATE(Window)
public:
    enum State {
        Inactive = 0,
        Active,
        Minimized,
        Maximized,
        Fullscreen
    };
    Q_ENUMS(State)
    Q_DECLARE_FLAGS(States, State)
    Q_FLAGS(States)

    explicit Window(const QString &title, const QString &identifier, States state, QObject *parent = 0);
    Window(QObject *parent = 0);
    ~Window();

    QString title() const;

    QString identifier() const;

    States state() const;
    void setState(const States &state);

Q_SIGNALS:
    void titleChanged(const QString &value);
    void identifierChanged(const QString &value);
    void stateChanged(const Window::States &value);
    void unmapped(Window *window);

public Q_SLOTS:
    void activate();
    void minimize();
    void unminimize();
    void maximize();
    void restore();

private:
    friend class WaylandIntegration;

    WindowPrivate *const d_ptr;
};

#endif // WINDOW_H
