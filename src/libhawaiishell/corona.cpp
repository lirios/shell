/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "corona_p.h"

namespace Hawaii {

namespace Shell {

/*
 * CoronaPrivate
 */

CoronaPrivate::CoronaPrivate(Corona *parent)
    : engine(new QQmlEngine(parent))
    , q_ptr(parent)
{
}

/*
 * Corona
 */

Corona::Corona(QObject *parent)
    : QObject(parent)
    , d_ptr(new CoronaPrivate(this))
{
}

Corona::~Corona()
{
    delete d_ptr;
}

QQmlEngine *Corona::engine() const
{
    Q_D(const Corona);
    return d->engine;
}

QString Corona::shell() const
{
    Q_D(const Corona);
    return d->shell;
}

void Corona::setShell(const QString &shell)
{
    Q_D(Corona);
    if (d->shell != shell) {
        d->shell = shell;
        Q_EMIT shellChanged(d->shell);
    }
}

Package Corona::package() const
{
    Q_D(const Corona);
    return d->package;
}

void Corona::setPackage(const Package &package)
{
    Q_D(Corona);
    d->package = package;
    Q_EMIT packageChanged(package);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_corona.cpp"
