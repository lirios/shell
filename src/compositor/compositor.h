/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QProcess>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

#include <GreenIsland/Compositor>

class Shell;
class ShellSurface;
class ClientWindow;
class Workspace;
class Grab;
class PanelManager;
class ScreenSaver;
class Notifications;

class Compositor : public GreenIsland::Compositor
{
    Q_OBJECT
    Q_ENUMS(ShellWindowRole)
public:
    enum ShellWindowRole {
        LockScreenRole = 0,
        OverlayWindowRole,
        DialogWindowRole,
        FullScreenWindowRole,
        PanelWindowRole,
        PopupWindowRole,
        NotificationsWindowRole,
        DesktopWindowRole,
        BackgroundWindowRole,
        ApplicationWindowRole
    };

    explicit Compositor();
    ~Compositor();

    static Compositor *instance();

    Shell *shell() const;
    ShellSurface *shellSurface() const;
    ScreenSaver *screenSaver() const;

    void surfaceCreated(QWaylandSurface *surface);

Q_SIGNALS:
    void ready();

    void windowAdded(QVariant window);
    void windowRemoved(QVariant window);
    void windowDestroyed(QVariant window);

    void fadeIn();
    void fadeOut();

    void locked();
    void unlocked();

public Q_SLOTS:
    void destroyWindow(QVariant window);
    void destroyClientForWindow(QVariant window);

    void lockSession();
    void unlockSession();

private Q_SLOTS:
    void surfaceMapped(QWaylandSurface *surface);
    void surfaceUnmapped(QWaylandSurface *surface);
    void surfaceDestroyed(QWaylandSurface *surface);

    void sceneGraphInitialized();
    void sendCallbacks();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY);

private:
    Notifications *m_notifications;
    ScreenSaver *m_screenSaver;
    PanelManager *m_panelManager;
    ShellSurface *m_shellSurface;
    Shell *m_shell;

    QList<ClientWindow *> m_clientWindows;
    QList<Workspace *> m_workspaces;

    bool m_shellReady;

    // Cursor
    QWaylandSurface *m_cursorSurface;
    int m_cursorHotspotX;
    int m_cursorHotspotY;

    QPointF calculateInitialPosition(QWaylandSurface *surface);
};

#endif // COMPOSITOR_H
