/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QTimer>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/private/qwlsurface_p.h>

#include "cmakedirs.h"
#include "compositor.h"
#include "logging.h"
#include "screensaver.h"
#include "shell.h"

ScreenSaver::ScreenSaver(wl_display *display, QObject *parent)
    : QObject(parent)
    , QtWaylandServer::wl_screensaver(display)
    , m_compositor(nullptr)
    , m_enabled(true)
    , m_timeout(5*60*1000)
    , m_timer(new QTimer(this))
    , m_process(new QProcess(this))
{
    m_timer->setInterval(m_timeout);
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(timeout()));

    connect(m_process, SIGNAL(started()),
            this, SLOT(processStarted()));
    connect(m_process, SIGNAL(finished(int)),
            this, SLOT(processFinished(int)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
}

void ScreenSaver::setCompositor(Compositor *compositor)
{
    m_compositor = compositor;
}

void ScreenSaver::launchProcess()
{
    // If the screen saver is disabled we just sleep
    if (!m_enabled) {
        m_compositor->setState(Compositor::Sleeping);
        return;
    }

    // Don't run another screen saver if the old one is
    // still running
    if (m_process->pid() != 0) {
        qCDebug(HAWAII_COMPOSITOR) << "Old screen saver is still running";
        return;
    }

    const char *exe = INSTALL_LIBEXECDIR "/hawaii-screensaver";
    if (!m_process->startDetached(QString::fromUtf8(exe)))
        qCWarning(HAWAII_COMPOSITOR) << "Unable to start the screen saver";
}

void ScreenSaver::terminateProcess()
{
    if (m_process->pid() == 0)
        return;

    m_process->terminate();
}

void ScreenSaver::screensaver_set_surface(Resource *resource,
                                          struct ::wl_resource *output_resource,
                                          struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    // TODO: As soon as QtCompositor handles outputs we need to center on output

    QWaylandSurfaceItem *item = surface->surfaceItem();
    if (item)
        item->setZ(100000);

    Q_EMIT m_compositor->fadeIn();
}

void ScreenSaver::timeout()
{
    // After a while we close the screen saver and sleep
    // in order to save some power
    qCDebug(HAWAII_COMPOSITOR) << "Screen saver timeout...";
    terminateProcess();
    m_compositor->setState(Compositor::Sleeping);
}

void ScreenSaver::processStarted()
{
    // Start the timer that when triggered will terminate
    // the screen saver and sleep
    qCDebug(HAWAII_COMPOSITOR) << "Screen saver started";
    m_timer->start();
}

void ScreenSaver::processFinished(int code)
{
    // Make sure the timeout won't trigger when the
    // screen saver is gone
    qCDebug(HAWAII_COMPOSITOR) << "Screen saver finished with exit code" << code;
    m_timer->stop();
}

void ScreenSaver::processError(QProcess::ProcessError error)
{
    Q_UNUSED(error);

    if (m_compositor->shell()->isLocked())
        m_compositor->setState(Compositor::Sleeping);
}
