/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtGui/QPlatformSurfaceEvent>
#include <QtGui/QWindow>

#include <LiriWaylandClient/Surface>
#include <LiriWaylandClient/private/registry_p.h>
#include <LiriWaylandClient/private/surface_p.h>

#include "shellhelperclient.h"

#include "qwayland-shell-helper.h"

using namespace Liri::WaylandClient;

class ShellHelperClientPrivate
        : public QtWayland::liri_shell
{
    Q_DECLARE_PUBLIC(ShellHelperClient)
public:
    ShellHelperClientPrivate(ShellHelperClient *qq)
        : QtWayland::liri_shell()
        , q_ptr(qq)
    {
    }

    void registerWindow(QWindow *window)
    {
        Surface *surface = Surface::fromQt(window);
        if (surface)
            set_grab_surface(SurfacePrivate::get(surface)->object());
    }

    quint32 name = 0;

protected:
    ShellHelperClient *q_ptr;

private:
    void liri_shell_grab_cursor(uint32_t cursor) override
    {
        Q_Q(ShellHelperClient);
        Q_EMIT q->cursorChangeRequested(static_cast<ShellHelperClient::GrabCursor>(cursor));
    }
};

ShellHelperClient::ShellHelperClient(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShellHelperClientPrivate(this))
{
}

ShellHelperClient::~ShellHelperClient()
{
    delete d_ptr;
}

quint32 ShellHelperClient::name() const
{
    Q_D(const ShellHelperClient);
    return d->name;
}

void ShellHelperClient::initialize(Registry *registry, quint32 name, quint32 version)
{
    Q_D(ShellHelperClient);
    d->name = name;
    d->init(RegistryPrivate::get(registry)->registry, name, version);
}

bool ShellHelperClient::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(ShellHelperClient);

    if (event->type() == QEvent::PlatformSurface
            && static_cast<QPlatformSurfaceEvent *>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
        QWindow *window = qobject_cast<QWindow*>(watched);
        Q_ASSERT(window);
        window->removeEventFilter(this);
        d->registerWindow(window);
    }

    return false;
}

void ShellHelperClient::registerGrabSurface(QWindow *window)
{
    Q_D(ShellHelperClient);

    if (window->handle())
        d->registerWindow(window);
    else
        window->installEventFilter(this);
}

QByteArray ShellHelperClient::interfaceName()
{
    return QByteArrayLiteral("liri_shell");
}
