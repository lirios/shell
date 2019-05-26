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
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QtWaylandCompositor/qwaylandcompositor.h>
#include <QtWaylandCompositor/QWaylandPointer>
#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandSurface>

#include "logging_p.h"
#include "shellhelper.h"
#include "shellhelper_p.h"

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
        qCDebug(lcShell) << "Stopping shell helper...";
        process->terminate();
        if (!process->waitForFinished())
            process->kill();
    }

    bool startProcess()
    {
        return runProgram(QString::asprintf("%s/liri-shell-helper", INSTALL_LIBEXECDIR));
    }

    QProcess *process;

private Q_SLOTS:
    void handleReadOutput()
    {
        qCInfo(lcShell) << process->readAllStandardOutput();
    }

    void handleReadError()
    {
        qCCritical(lcShell) << process->readAllStandardError();
    }

private:
    bool runProgram(const QString &path)
    {
        if (!QFile::exists(path))
            return false;

        int retries = 5;
        while (retries-- > 0) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            //env.insert(QLatin1String("WAYLAND_DEBUG"), "1");
            process->setProcessEnvironment(env);
            process->start(path);
            qCDebug(lcShell, "Trying shell helper (%s)...", qPrintable(path));
            if (Q_LIKELY(process->waitForStarted())) {
                qCInfo(lcShell, "Running shell helper (%s)...", qPrintable(path));
                return true;
            } else {
                if (retries == 0)
                    qCWarning(lcShell, "Failed to start shell helper, giving up");
                else
                    qCWarning(lcShell, "Failed to start shell helper, %d attempt(s) left",
                              retries);
            }
        }

        return false;
    }
};

/*
 * ShellHelperPrivate
 */

ShellHelperPrivate::ShellHelperPrivate(ShellHelper *qq)
    : QtWaylandServer::liri_shell()
    , runnerThread(new QThread())
    , processRunner(new ProcessRunner())
    , q_ptr(qq)
{
    processRunner->moveToThread(runnerThread);
}

ShellHelperPrivate::~ShellHelperPrivate()
{
    runnerThread->wait();
    runnerThread->quit();
    delete runnerThread;

    delete processRunner;
}

ShellHelperPrivate *ShellHelperPrivate::get(ShellHelper *shell)
{
    return shell->d_func();
}

void ShellHelperPrivate::liri_shell_bind_resource(Resource *r)
{
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
        qCWarning(lcShell) << "Couldn't find surface from resource";
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
        qCWarning(lcShell) << "Failed to find QWaylandCompositor when initializing ShellHelper";
        return;
    }
    d->init(compositor->display(), 1);
}

void ShellHelper::start()
{
    Q_D(ShellHelper);

    // Run the shell helper in a thread to avoid
    // blocking the compositor
    if (d->processRunner->startProcess())
        Q_EMIT processStarted();
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

#include "shellhelper.moc"
