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
#include <QtGui/qpa/qwindowsysteminterface.h>

#include <LiriWaylandClient/ClientConnection>
#include <LiriWaylandClient/Registry>

#include "shellhelperapplication.h"

using namespace Liri::WaylandClient;

class ShellHelperApplicationPrivate
{
public:
    ShellHelperApplicationPrivate()
        : thread(new QThread())
        , clientConnection(ClientConnection::fromQt())
        , registry(new Registry())
        , grabWindow(new QWindow())
    {
        clientConnection->moveToThread(thread);
        thread->start();

        grabWindow->setFlags(Qt::BypassWindowManagerHint);
        grabWindow->resize(1, 1);
    }

    ~ShellHelperApplicationPrivate()
    {
        delete helper;
        delete grabWindow;
        delete registry;
        delete clientConnection;

        thread->quit();
        thread->wait();
        delete thread;
    }

    QThread *thread;
    ClientConnection *clientConnection;
    Registry *registry;
    QWindow *grabWindow;
    ShellHelperClient *helper = nullptr;
};

ShellHelperApplication::ShellHelperApplication(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShellHelperApplicationPrivate())
{
    initialize();
}

ShellHelperApplication::~ShellHelperApplication()
{
    delete d_ptr;
}

void ShellHelperApplication::initialize()
{
    Q_D(ShellHelperApplication);

    connect(d->registry, &Registry::interfaceAnnounced, this,
            [this, d](const QByteArray &interface, quint32 name, quint32 version) {
        if (interface == ShellHelperClient::interfaceName()) {
            d->helper = new ShellHelperClient();
            d->helper->initialize(d->registry, name, version);

            connect(d->helper, &ShellHelperClient::cursorChangeRequested,
                    this, &ShellHelperApplication::handleCursorChangeRequest);
        }
    });

    connect(d->registry, &Registry::interfacesAnnounced, this, [d] {
        // All interfaces were announced including shm, let's start
        d->grabWindow->show();
        d->helper->registerGrabSurface(d->grabWindow);
    });

    connect(d->registry, &Registry::interfaceRemoved, this,
            [d](quint32 name) {
        if (d->helper && d->helper->name() == name) {
            delete d->helper;
            d->helper = nullptr;
        }
    });

    d->registry->create(d->clientConnection->display());
    d->registry->setup();
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

    QWindowSystemInterface::handleEnterLeaveEvent(d->grabWindow, d->grabWindow, QPointF(0, 0), QPointF(0, 0));
}
