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

#include <QtCore/QDebug>
#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "desktopview.h"
#include "shellclient.h"
#include "shellmanager.h"
#include "shellcontroller_p.h"
#include "window.h"
#include "window_p.h"
#include "workspace.h"
#include "workspace_p.h"

ShellClient::ShellClient(QObject *parent)
    : QObject(parent)
    , QtWayland::hawaii_shell()
{
}

void ShellClient::shell_loaded()
{
    Q_EMIT loaded();
}

void ShellClient::shell_configure(struct ::wl_surface *target,
                                         int32_t width,
                                         int32_t height)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    QList<DesktopView *> views =
            ShellManager::instance()->mantle()->desktops();
    for (DesktopView *view: views) {
        struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                    native->nativeResourceForWindow("surface", view));
        if (target == surface) {
            qDebug("-> Resize DesktopView to %dx%d", width, height);
            view->resize(width, height);
            return;
        }
    }
}

void ShellClient::shell_prepare_lock_surface()
{
    Q_EMIT prepareLockSurface();
}

void ShellClient::shell_grab_cursor(uint32_t cursor)
{
    QCursor qcursor;

    switch (cursor) {
    case HAWAII_SHELL_CURSOR_NONE:
        break;
    case HAWAII_SHELL_CURSOR_BUSY:
        qcursor.setShape(Qt::BusyCursor);
        break;
    case HAWAII_SHELL_CURSOR_MOVE:
        qcursor.setShape(Qt::DragMoveCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_TOP:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_BOTTOM:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_LEFT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_RIGHT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_TOP_LEFT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_TOP_RIGHT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_BOTTOM_LEFT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_SHELL_CURSOR_RESIZE_BOTTOM_RIGHT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_SHELL_CURSOR_ARROW:
    default:
        qcursor.setShape(Qt::ArrowCursor);
        break;
    }

    Q_EMIT cursorChanged(qcursor);
}

void ShellClient::shell_window_mapped(struct ::hawaii_window *id,
                                             const QString &title,
                                             const QString &identifier,
                                             int32_t state)
{
    Window *window = new Window(title, identifier, wlStateConvert(state));
    window->d_ptr->init(id);
    window->moveToThread(QCoreApplication::instance()->thread());

    Q_EMIT windowAdded(window);
}

void ShellClient::shell_window_switching_started()
{
    Q_EMIT windowSwitchingStarted();
}

void ShellClient::shell_window_switching_finished()
{
    Q_EMIT windowSwitchingFinished();
}

void ShellClient::shell_window_switched(struct ::hawaii_window *window)
{
    QList<Window *> list = ShellManager::instance()->controller()->d_ptr->windowsList;

    for (int i = 0; i < list.size(); i++) {
        Window *w = list.at(i);
        if (w->d_ptr->object() == window) {
            Q_EMIT switchedToWindow(w);
            break;
        }
    }
}

void ShellClient::shell_workspace_added(struct ::hawaii_workspace *id,
                                               int32_t active)
{
    Workspace *workspace = new Workspace(active != 0);
    workspace->d_ptr->init(id);
    workspace->moveToThread(QCoreApplication::instance()->thread());

    Q_EMIT workspaceAdded(workspace);
}

#include "moc_shellclient.cpp"
