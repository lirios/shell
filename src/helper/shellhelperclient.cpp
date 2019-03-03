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

#include <QtGui/QGuiApplication>
#include <QtGui/QPlatformSurfaceEvent>
#include <QtGui/QWindow>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "shellhelperclient.h"

#include "qwayland-shell-helper.h"

static inline struct ::wl_surface *getWlSurface(QWindow *window)
{
    void *surface = QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window);
    return static_cast<struct ::wl_surface *>(surface);
}

class ShellHelperClientPrivate : public QtWayland::liri_shell
{
    Q_DECLARE_PUBLIC(ShellHelperClient)
public:
    ShellHelperClientPrivate(ShellHelperClient *qq)
        : QtWayland::liri_shell()
        , q_ptr(qq)
    {
    }

    void sendGrabSurfaceRegistration(QWindow *window)
    {
        Q_Q(ShellHelperClient);

        if (window->handle())
            setGrabSurface(window);
        else
            window->installEventFilter(q);

        activated = true;
    }

    void setGrabSurface(QWindow *window)
    {
        set_grab_surface(getWlSurface(window));
        window->setCursor(QCursor(Qt::ArrowCursor));
    }

    bool activated = false;
    QWindow *grabWindow = nullptr;

protected:
    ShellHelperClient *q_ptr;

private:
    void liri_shell_grab_cursor(uint32_t cursor) override
    {
        Q_Q(ShellHelperClient);
        Q_EMIT q->cursorChangeRequested(static_cast<ShellHelperClient::GrabCursor>(cursor));
    }
};

ShellHelperClient::ShellHelperClient()
    : QWaylandClientExtensionTemplate(1)
    , d_ptr(new ShellHelperClientPrivate(this))
{
    connect(this, &ShellHelperClient::activeChanged, this, &ShellHelperClient::handleActivation);
}

ShellHelperClient::~ShellHelperClient()
{
    delete d_ptr;
}

void ShellHelperClient::init(wl_registry *registry, int id, int version)
{
    Q_D(ShellHelperClient);
    d->init(registry, id, version);
}

void ShellHelperClient::registerGrabSurface(QWindow *window)
{
    Q_D(ShellHelperClient);

    d->grabWindow = window;
    if (isActive())
        d->sendGrabSurfaceRegistration(window);
}

const wl_interface *ShellHelperClient::interface()
{
    return QtWayland::liri_shell::interface();
}

bool ShellHelperClient::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(ShellHelperClient);

    if (event->type() == QEvent::PlatformSurface
            && static_cast<QPlatformSurfaceEvent *>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
        QWindow *window = qobject_cast<QWindow*>(watched);
        Q_ASSERT(window);
        window->removeEventFilter(this);
        d->setGrabSurface(window);
    }

    return false;
}

void ShellHelperClient::handleActivation()
{
    Q_D(ShellHelperClient);

    if (isActive() && !d->activated && d->grabWindow)
        d->sendGrabSurfaceRegistration(d->grabWindow);
}
