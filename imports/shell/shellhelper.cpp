/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QtWaylandCompositor/qwaylandcompositor.h>
#include <QtWaylandCompositor/QWaylandPointer>
#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/private/qwaylandcompositorextension_p.h>

#include "cmakedirs.h"
#include "logging_p.h"
#include "shellhelper.h"

#include "qwayland-server-shell-helper.h"

class ProcessRunner : public QObject
{
    Q_OBJECT
public:
    ProcessRunner(QObject *parent = nullptr)
        : QObject(parent)
        , process(new QProcess(this))
    {
        process->setProcessChannelMode(QProcess::ForwardedChannels);

        connect(process, SIGNAL(readyReadStandardOutput()),
                this, SLOT(handleReadOutput()));
        connect(process, SIGNAL(readyReadStandardError()),
                this, SLOT(handleReadError()));
    }

    ~ProcessRunner()
    {
        qCDebug(gLcShell) << "Stopping shell helper...";
        process->terminate();
        if (!process->waitForFinished())
            process->kill();
    }

    void startProcess(const QString &socketName)
    {
        int retries = 5;
        while (retries-- > 0) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert(QLatin1String("WAYLAND_DISPLAY"), socketName);
            //env.insert(QLatin1String("WAYLAND_DEBUG"), "1");
            process->setProcessEnvironment(env);

            qCDebug(gLcShell) << "Starting shell helper...";
            process->start(QLatin1String(INSTALL_LIBEXECDIR "/liri-shell-helper"));
            if (Q_LIKELY(process->waitForStarted())) {
                return;
            } else {
                if (retries == 0)
                    qCWarning(gLcShell, "Failed to start shell helper, giving up");
                else
                    qCWarning(gLcShell, "Failed to start shell helper, %d attempt(s) left",
                              retries);
            }
        }
    }

    QProcess *process;

private Q_SLOTS:
    void handleReadOutput()
    {
        qCInfo(gLcShell) << process->readAllStandardOutput();
    }

    void handleReadError()
    {
        qCCritical(gLcShell) << process->readAllStandardError();
    }
};

class ShellHelperPrivate
        : public QWaylandCompositorExtensionPrivate
        , public QtWaylandServer::liri_shell
{
    Q_DECLARE_PUBLIC(ShellHelper)
public:
    ShellHelperPrivate()
        : QWaylandCompositorExtensionPrivate()
        , QtWaylandServer::liri_shell()
        , runnerThread(new QThread())
        , processRunner(new ProcessRunner())
    {
        processRunner->moveToThread(runnerThread);
    }

    ~ShellHelperPrivate()
    {
        runnerThread->wait();
        runnerThread->quit();
        delete runnerThread;

        delete processRunner;
    }

    static ShellHelperPrivate *get(ShellHelper *shell)
    {
        return shell->d_func();
    }

    QThread *runnerThread;
    ProcessRunner *processRunner;
    QWaylandSurface *grabSurface = nullptr;

private:
    void liri_shell_bind_resource(Resource *r) Q_DECL_OVERRIDE
    {
        // Client can bind only once
        if (resource())
            wl_resource_post_error(r->handle,
                                   WL_DISPLAY_ERROR_INVALID_OBJECT,
                                   "client can bind only once");
    }

    void liri_shell_set_grab_surface(Resource *resource, struct ::wl_resource *wlSurface) Q_DECL_OVERRIDE
    {
        Q_Q(ShellHelper);

        auto surface = QWaylandSurface::fromResource(wlSurface);
        if (surface) {
            grabSurface = surface;
            Q_EMIT q->grabSurfaceAdded(surface);
        } else {
            qCWarning(gLcShell) << "Couldn't find surface from resource";
            wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                                   "the specified surface is invalid");
        }
    }
};

ShellHelper::ShellHelper()
    : QWaylandCompositorExtensionTemplate<ShellHelper>(*new ShellHelperPrivate())
{
}

ShellHelper::ShellHelper(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<ShellHelper>(compositor, *new ShellHelperPrivate())
{
}

void ShellHelper::initialize()
{
    Q_D(ShellHelper);

    QWaylandCompositorExtension::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcShell) << "Failed to find QWaylandCompositor when initializing ShellHelper";
        return;
    }
    d->init(compositor->display(), 1);
}

void ShellHelper::start(const QString &socketName)
{
    Q_D(ShellHelper);

    // Run the shell helper in a thread to avoid
    // blocking the compositor
    d->processRunner->startProcess(socketName);
}

void ShellHelper::grabCursor(GrabCursor cursor)
{
    Q_D(ShellHelper);

    if (d->grabSurface) {
        auto resource = d->resourceMap().value(d->grabSurface->waylandClient());
        if (resource) {
            auto seat = d->grabSurface->compositor()->defaultSeat();
            auto pointer = seat->pointer();
            auto view = d->grabSurface->views().at(0);
            if (pointer && view) {
                pointer->sendMouseMoveEvent(view, QPointF(0, 0), QPointF(0, 0));
                d->send_grab_cursor(resource->handle, static_cast<uint32_t>(cursor));
            }
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

#include "shellhelper.moc"
