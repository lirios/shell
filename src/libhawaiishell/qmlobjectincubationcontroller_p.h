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

#ifndef QMLOBJECTINCUBATIONCONTROLLER_P_H
#define QMLOBJECTINCUBATIONCONTROLLER_P_H

#include <QtCore/QObject>
#include <QtQml/QQmlIncubationController>

class QmlObjectIncubationController : public QObject, public QQmlIncubationController
{
    Q_OBJECT
public:
    QmlObjectIncubationController(QObject *parent);

public Q_SLOTS:
    void incubate();

protected:
    virtual bool event(QEvent *event);

    void incubatingObjectCountChanged(int count);

private:
    int m_incubationTime;
};

#endif // QMLOBJECTINCUBATIONCONTROLLER_P_H
