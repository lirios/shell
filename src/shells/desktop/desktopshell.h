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
#include <QtCore/QHash>
#include <QtQml/QQmlListProperty>

class QQmlEngine;

class ShellUi;
class Window;
class Workspace;
class KeyBinding;

class DesktopShell : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Window> windows READ windows NOTIFY windowsChanged)
    Q_PROPERTY(QQmlListProperty<Workspace> workspaces READ workspaces NOTIFY workspacesChanged)
public:
    DesktopShell();
    ~DesktopShell();

    static DesktopShell *instance();

    inline QQmlEngine *engine() const {
        return m_engine;
    }

    Q_INVOKABLE QObject *service(const QString &name);

    Q_INVOKABLE KeyBinding *addKeyBinding(quint32 key, quint32 modifiers);

    inline QList<ShellUi *> shellWindows() const {
        return m_shellWindows;
    }

    QQmlListProperty<Window> windows();
    QQmlListProperty<Workspace> workspaces();

Q_SIGNALS:
    void windowsChanged();
    void workspaceAdded(int num);
    void workspaceRemoved(int num);
    void workspacesChanged();

public Q_SLOTS:
    void create();
    void ready();

    void minimizeWindows();
    void restoreWindows();

    void addWorkspace();
    void removeWorkspace(int num);

private:
    friend class WaylandIntegration;

    struct wl_display *m_display;
    int m_fd;
    struct wl_registry *m_registry;
    QElapsedTimer m_elapsedTimer;
    QQmlEngine *m_engine;
    QHash<QString, QObject *> m_services;
    QList<ShellUi *> m_shellWindows;
    QList<Window *> m_windows;
    QList<Workspace *> m_workspaces;
    QList<KeyBinding *> m_keyBindings;

    void appendWindow(Window *window);
    void appendWorkspace(Workspace *workspace);

    static int windowsCount(QQmlListProperty<Window> *p);
    static Window *windowAt(QQmlListProperty<Window> *p, int index);

    static int workspacesCount(QQmlListProperty<Workspace> *p);
    static Workspace *workspaceAt(QQmlListProperty<Workspace> *p, int index);

private Q_SLOTS:
    void windowUnmapped(Window *window);
};

#endif // DESKTOPSHELL_H
