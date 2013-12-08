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

#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QProcess>


#include <GreenIsland/Compositor>

class Shell;
class ClientWindow;
class Workspace;
class Grab;
class Notifications;

class Compositor : public GreenIsland::Compositor
{
    Q_OBJECT
public:
    explicit Compositor();
    ~Compositor();

    static Compositor *instance();

    Shell *shell() const;

    bool isShellWindow(QWaylandSurface *surface);

    void surfaceCreated(QWaylandSurface *surface);

Q_SIGNALS:
    void ready();

public Q_SLOTS:
    void destroyWindow(QVariant window);
    void destroyClientForWindow(QVariant window);

private Q_SLOTS:
    void surfaceMapped(QWaylandSurface *surface);
    void surfaceUnmapped(QWaylandSurface *surface);
    void surfaceDestroyed(QWaylandSurface *surface);

    void sceneGraphInitialized();
    void frameSwapped();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY);

private:
    Shell *m_shell;
    QList<ClientWindow *> m_clientWindows;
    QList<Workspace *> m_workspaces;
    Notifications *m_notifications;
    bool m_shellReady;

    // Cursor
    QWaylandSurface *m_cursorSurface;
    int m_cursorHotspotX;
    int m_cursorHotspotY;

    QPointF calculateInitialPosition(QWaylandSurface *surface);
};

#endif // COMPOSITOR_H
