/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include "applicationaction.h"

class ApplicationActionPrivate
{
public:
    ApplicationActionPrivate()
    {
    }

    QString name;
    QString iconName;
    QString command;
};

ApplicationAction::ApplicationAction(const QString &name, const QString &iconName,
                                     const QString &command, QObject *parent)
    : QObject(parent)
    , d_ptr(new ApplicationActionPrivate())
{
    Q_D(ApplicationAction);
    d->name = name;
    d->iconName = iconName;
    d->command = command;
}

ApplicationAction::~ApplicationAction()
{
    delete d_ptr;
}

QString ApplicationAction::name() const
{
    Q_D(const ApplicationAction);
    return d->name;
}

QString ApplicationAction::iconName() const
{
    Q_D(const ApplicationAction);
    return d->iconName;
}

QString ApplicationAction::command() const
{
    Q_D(const ApplicationAction);
    return d->command;
}

#include "moc_applicationaction.cpp"
