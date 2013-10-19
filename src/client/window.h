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
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlPropertyMap>

class AppInfo;
class HawaiiShellImpl;
class WindowPrivate;

class Window : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString identifier READ identifier NOTIFY identifierChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(bool minimized READ isMinimized NOTIFY minimizedChanged)
    Q_PROPERTY(bool maximized READ isMaximized NOTIFY maximizedChanged)
    Q_PROPERTY(bool fullscreen READ isFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(AppInfo *appInfo READ appInfo NOTIFY appInfoChanged)
    Q_DISABLE_COPY(Window)
    Q_DECLARE_PRIVATE(Window)
    Q_ENUMS(State)
    Q_FLAGS(States)
public:
    enum State {
        Inactive = 0x00,
        Active = 0x01,
        Minimized = 0x02,
        Maximized = 0x04,
        Fullscreen = 0x08
    };
    Q_DECLARE_FLAGS(States, State)

    explicit Window(const QString &title, const QString &identifier, States state, QObject *parent = 0);
    Window(QObject *parent = 0);
    ~Window();

    QString title() const;

    QString identifier() const;

    bool isActive() const;
    bool isMinimized() const;
    bool isMaximized() const;
    bool isFullscreen() const;

    States state() const;
    void setState(const States &state);

    AppInfo *appInfo() const;

    static QQmlPropertyMap *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void titleChanged(const QString &value);
    void identifierChanged(const QString &value);
    void activeChanged(bool value);
    void minimizedChanged(bool value);
    bool maximizedChanged(bool value);
    bool fullscreenChanged(bool value);
    void stateChanged(const Window::States &value);
    void appInfoChanged();
    void unmapped(Window *window);

public Q_SLOTS:
    void activate();
    void deactivate();

    void minimize();
    void unminimize();

    void maximize();
    void restore();

private:
    friend class HawaiiShellImpl;

    WindowPrivate *const d_ptr;
};

#endif // WINDOW_H
