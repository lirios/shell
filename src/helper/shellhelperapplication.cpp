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
}

ShellHelperApplication::~ShellHelperApplication()
{
    delete d_ptr;
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
