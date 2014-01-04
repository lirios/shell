/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef HAWAIISHELL_H
#define HAWAIISHELL_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtQml/QQmlListProperty>

class QScreen;
class QQmlEngine;

class HawaiiShellImpl;
class HawaiiShellPrivate;
class ShellUi;
class Window;
class Workspace;
class KeyBinding;
class LauncherWindow;
class GrabWindow;
class DialogQuickWindow;
class ShellWindow;
class PopupQuickWindow;
class OverlayWindow;
class LockScreenWindow;
class NotificationWindow;
class SessionManager;

class HawaiiShell : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Window> windows READ windows NOTIFY windowsChanged)
    Q_PROPERTY(QQmlListProperty<Workspace> workspaces READ workspaces NOTIFY workspacesChanged)
    Q_DECLARE_PRIVATE(HawaiiShell)
public:
    HawaiiShell();
    ~HawaiiShell();

    static HawaiiShell *instance();

    QQmlEngine *engine() const;

    Q_INVOKABLE QObject *service(const QString &name);

    Q_INVOKABLE KeyBinding *addKeyBinding(quint32 key, quint32 modifiers);

    Q_INVOKABLE void setAvailableGeometry(QScreen *screen, const QRect &geometry);

    ShellUi *uiController() const;

    QQmlListProperty<Window> windows();
    QQmlListProperty<Workspace> workspaces();

Q_SIGNALS:
    void windowsChanged();
    void windowActivated(Window *window);
    void windowSwitchingStarted();
    void windowSwitchingFinished();
    void windowSwitchingNext(Window *window);

    void workspaceAdded(int num);
    void workspaceRemoved(int num);
    void workspacesChanged();
    void workspaceSwitched(Workspace *workspace);

public Q_SLOTS:
    void create();

    void minimizeWindows();
    void restoreWindows();
    void toggleWindows();

    void addWorkspace();
    void addWorkspaces(int num);

    void removeWorkspace(int num);

    void selectWorkspace(Workspace *workspace);

private:
    friend class HawaiiShellImpl;
    friend class LauncherWindow;
    friend class GrabWindow;
    friend class DialogQuickWindow;
    friend class ShellWindow;
    friend class PopupQuickWindow;
    friend class OverlayWindow;
    friend class LockScreenWindow;
    friend class NotificationWindow;
    friend class SessionManager;

    HawaiiShellPrivate *const d_ptr;

    static int windowsCount(QQmlListProperty<Window> *p);
    static Window *windowAt(QQmlListProperty<Window> *p, int index);

    static int workspacesCount(QQmlListProperty<Workspace> *p);
    static Workspace *workspaceAt(QQmlListProperty<Workspace> *p, int index);

private Q_SLOTS:
    void windowUnmapped(Window *window);
};

#endif // HAWAIISHELL_H
