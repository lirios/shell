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

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QtCore/QObject>
#include <QtCore/QProcess>

#include "qwayland-server-screensaver.h"

class QTimer;
class Compositor;

class ScreenSaver : public QObject, public QtWaylandServer::wl_screensaver
{
    Q_OBJECT
public:
    explicit ScreenSaver(struct ::wl_display *display,
                         QObject *parent = 0);

    void setCompositor(Compositor *compositor);

    void launchProcess();
    void terminateProcess();

protected:
    void screensaver_set_surface(Resource *resource,
                                 struct ::wl_resource *output_resource,
                                 struct ::wl_resource *surface_resource) Q_DECL_OVERRIDE;

private:
    Compositor *m_compositor;
    bool m_enabled;
    int m_timeout;
    QTimer *m_timer;
    QProcess *m_process;

private Q_SLOTS:
    void timeout();

    void processStarted();
    void processFinished(int code);
    void processError(QProcess::ProcessError error);
};

#endif // SCREENSAVER_H
