/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2+$
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "qmlobjectincubationcontroller_p.h"

QmlObjectIncubationController::QmlObjectIncubationController(QObject *parent)
    : QObject(parent)
    , QQmlIncubationController()
{
    // Allow incubation for 1/3 of a frame
    m_incubationTime = qMax(1, int(1000 / QGuiApplication::primaryScreen()->refreshRate()) / 3);
}

void QmlObjectIncubationController::incubate()
{
    if (incubatingObjectCount()) {
        incubateFor(m_incubationTime * 2);
        if (incubatingObjectCount())
            QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }
}

bool QmlObjectIncubationController::event(QEvent *event)
{
    if (event->type() == QEvent::User) {
        incubate();
        return true;
    }

    return QObject::event(event);
}

void QmlObjectIncubationController::incubatingObjectCountChanged(int count)
{
    if (count)
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

#include "moc_qmlobjectincubationcontroller_p.cpp"
