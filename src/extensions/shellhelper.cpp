/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QtCore/QFile>

#include <QtWaylandCompositor/qwaylandcompositor.h>
#include <QtWaylandCompositor/QWaylandPointer>
#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandSurface>

#include "shellhelper.h"
#include "shellhelper_p.h"

Q_LOGGING_CATEGORY(lcShellHelper, "liri.shell", QtInfoMsg)

#ifndef WL_DISPLAY_ERROR_IMPLEMENTATION
#  define WL_DISPLAY_ERROR_IMPLEMENTATION WL_DISPLAY_ERROR_INVALID_METHOD
#endif

/*
 * ShellHelperPrivate
 */

ShellHelperPrivate::ShellHelperPrivate(ShellHelper *qq)
    : QtWaylandServer::liri_shell()
    , q_ptr(qq)
{
}

ShellHelperPrivate *ShellHelperPrivate::get(ShellHelper *shell)
{
    return shell->d_func();
}

void ShellHelperPrivate::liri_shell_bind_resource(Resource *r)
{
    // Make sure only an authorized client can bind
    pid_t pid;
    wl_client_get_credentials(r->client(), &pid, nullptr, nullptr);
    QFile file(QString::asprintf("/proc/%d/cmdline", pid));
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString data = stream.readAll();
        QStringList args = data.split(QLatin1Char('\0'));
        file.close();

        if (!args.first().endsWith(QStringLiteral("liri-shell-helper"))) {
            wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                                   "unauthorized client program");
            return;
        }
    } else {
        wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                               "unauthorized client program");
        return;
    }

    // Client can bind only once
    if (resource())
        wl_resource_post_error(r->handle,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "client can bind only once");
}

void ShellHelperPrivate::liri_shell_set_grab_surface(Resource *resource, struct ::wl_resource *wlSurface)
{
    Q_Q(ShellHelper);

    auto surface = QWaylandSurface::fromResource(wlSurface);
    if (surface) {
        grabSurface = surface;
        Q_EMIT q->grabSurfaceAdded(surface);
    } else {
        qCWarning(lcShellHelper) << "Couldn't find surface from resource";
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "the specified surface is invalid");
    }
}

/*
 * ShellHelper
 */

ShellHelper::ShellHelper()
    : QWaylandCompositorExtensionTemplate<ShellHelper>()
    , d_ptr(new ShellHelperPrivate(this))
{
}

ShellHelper::ShellHelper(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<ShellHelper>(compositor)
    , d_ptr(new ShellHelperPrivate(this))
{
}

ShellHelper::~ShellHelper()
{
    delete d_ptr;
}

void ShellHelper::initialize()
{
    Q_D(ShellHelper);

    QWaylandCompositorExtension::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(lcShellHelper) << "Failed to find QWaylandCompositor when initializing ShellHelper";
        return;
    }
    d->init(compositor->display(), 1);
}

void ShellHelper::grabCursor(GrabCursor cursor)
{
    Q_D(ShellHelper);

    if (d->grabSurface) {
        auto resource = d->resourceMap().value(d->grabSurface->waylandClient());
        if (resource)
            d->send_grab_cursor(resource->handle, static_cast<uint32_t>(cursor));
        if (d->grabSurface->views().size() > 0) {
            auto seat = d->grabSurface->compositor()->defaultSeat();
            auto view = d->grabSurface->views().at(0);
            seat->setMouseFocus(view);
            seat->sendMouseMoveEvent(view, QPointF(0, 0), QPointF(0, 0));
        }
    }
}

const struct wl_interface *ShellHelper::interface()
{
    return ShellHelperPrivate::interface();
}

QByteArray ShellHelper::interfaceName()
{
    return ShellHelperPrivate::interfaceName();
}
