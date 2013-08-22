/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef DESKTOPSHELL_H
#define DESKTOPSHELL_H

#include <QtCore/QObject>
#include <QtCore/QElapsedTimer>

class QQmlEngine;

class ShellUi;
class Window;
class Workspace;
class KeyBinding;

class DesktopShell : public QObject
{
    Q_OBJECT
public:
    DesktopShell();
    ~DesktopShell();

    static DesktopShell *instance();

    QQmlEngine *engine() const {
        return m_engine;
    }

    inline QList<ShellUi *> shellWindows() const {
        return m_shellWindows;
    }

    inline QList<Window *> windows() const {
        return m_windows;
    }

    void appendWindow(Window *window);

    void minimizeWindows();
    void restoreWindows();

    void addWorkspace();
    void removeWorkspace(int num);

    void appendWorkspace(Workspace *workspace);

    KeyBinding *addKeyBinding(quint32 key, quint32 modifiers);

Q_SIGNALS:
    void windowsChanged();
    void workspaceAdded(int num);
    void workspaceRemoved(int num);

public Q_SLOTS:
    void create();
    void ready();

private:
    struct wl_display *m_display;
    int m_fd;
    struct wl_registry *m_registry;
    QElapsedTimer m_elapsedTimer;
    QQmlEngine *m_engine;
    QList<ShellUi *> m_shellWindows;
    QList<Window *> m_windows;
    QList<Workspace *> m_workspaces;
    QList<KeyBinding *> m_keyBindings;

private Q_SLOTS:
    void windowUnmapped(Window *window);
};

#endif // DESKTOPSHELL_H
