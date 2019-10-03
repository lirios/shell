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

#include <QtCore/QThread>
#include <QtGui/QWindow>

#ifdef HAVE_SYSTEMD
#  include <QTimer>
#  include <systemd/sd-daemon.h>
#endif

#include "shellhelperapplication.h"

class ShellHelperApplicationPrivate
{
public:
    ShellHelperApplicationPrivate()
    {
        helper = new ShellHelperClient();

        grabWindow = new QWindow();
        grabWindow->setFlags(Qt::BypassWindowManagerHint);
        grabWindow->resize(1, 1);
        grabWindow->show();
        helper->registerGrabSurface(grabWindow);
    }

    ~ShellHelperApplicationPrivate()
    {
        delete helper;
        delete grabWindow;
    }

    ShellHelperClient *helper = nullptr;
    QWindow *grabWindow = nullptr;
};

ShellHelperApplication::ShellHelperApplication(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShellHelperApplicationPrivate())
{
    connect(d_ptr->helper, &ShellHelperClient::cursorChangeRequested,
            this, &ShellHelperApplication::handleCursorChangeRequest);

#ifdef HAVE_SYSTEMD
    setupSystemd();
#endif
}

ShellHelperApplication::~ShellHelperApplication()
{
    delete d_ptr;

#ifdef HAVE_SYSTEMD
    sd_notify(0, "STOPPING=1");
#endif
}

void ShellHelperApplication::handleCursorChangeRequest(ShellHelperClient::GrabCursor cursor)
{
    Q_D(ShellHelperApplication);

    QCursor newCursor;

    switch (cursor) {
    case ShellHelperClient::ArrowGrabCursor:
        newCursor.setShape(Qt::ArrowCursor);
        break;
    case ShellHelperClient::ResizeTopGrabCursor:
    case ShellHelperClient::ResizeBottomGrabCursor:
        newCursor.setShape(Qt::SizeVerCursor);
        break;
    case ShellHelperClient::ResizeLeftGrabCursor:
    case ShellHelperClient::ResizeRightGrabCursor:
        newCursor.setShape(Qt::SizeHorCursor);
        break;
    case ShellHelperClient::ResizeTopLeftGrabCursor:
    case ShellHelperClient::ResizeBottomRightGrabCursor:
        newCursor.setShape(Qt::SizeFDiagCursor);
        break;
    case ShellHelperClient::ResizeTopRightGrabCursor:
    case ShellHelperClient::ResizeBottomLeftGrabCursor:
        newCursor.setShape(Qt::SizeBDiagCursor);
        break;
    case ShellHelperClient::MoveGrabCursor:
        newCursor.setShape(Qt::DragMoveCursor);
        break;
    case ShellHelperClient::BusyGrabCursor:
        newCursor.setShape(Qt::BusyCursor);
        break;
    default:
        newCursor.setShape(Qt::BlankCursor);
        break;
    }

    d->grabWindow->setCursor(newCursor);
}

#ifdef HAVE_SYSTEMD
void ShellHelperApplication::setupSystemd()
{
    uint64_t interval = 0;
    if (sd_watchdog_enabled(0, &interval) > 0) {
        if (interval > 0) {
            // Start a keep-alive timer every half of the watchdog interval,
            // and convert it from microseconds to milliseconds
            std::chrono::microseconds us(interval / 2);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(us);
            auto *timer = new QTimer(this);
            timer->setInterval(ms);
            connect(timer, &QTimer::timeout,
                    this, &ShellHelperApplication::keepAlive);
            timer->start();
        }
    }

    sd_notify(0, "READY=1");
}

void ShellHelperApplication::keepAlive()
{
    sd_notify(0, "WATCHDOG=1");
}
#endif
