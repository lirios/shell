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

#include <QtQml/QQmlPropertyMap>

#include <QtConfiguration/QConfiguration>

#include "containment.h"

namespace Hawaii {

namespace Shell {

/*
 * ContainmentPrivate
 */

class ContainmentPrivate
{
public:
    ContainmentPrivate();

    QQmlPropertyMap settings;
    QConfiguration *configuration;
    Corona *corona;
    Types::FormFactor formFactor;
    Types::Location location;
};

ContainmentPrivate::ContainmentPrivate()
    : corona(nullptr)
    , formFactor(Types::Plane)
    , location(Types::Desktop)
{
}

/*
 * Containment
 */

Containment::Containment(Corona *corona, QObject *parent)
    : QObject(parent)
    , d_ptr(new ContainmentPrivate())
{
    Q_D(Containment);
    d->corona = corona;

    // Default configuration
    d->settings.insert("formFactor", d->formFactor);
    d->settings.insert("location", d->location);

#if 0
    // Save and load settings
    static int containmentId = 0;
    const QString section = QString("shell/%1/containments/containment%2")
            .arg(corona->shell())
            .arg(QString::number(containmentId++));
    d->configuration = new QConfiguration(&d->settings, section, this);
#endif
}

Containment::~Containment()
{
    delete d_ptr;
}

Corona *Containment::corona() const
{
    Q_D(const Containment);
    return d->corona;
}

Hawaii::Shell::Types::FormFactor Containment::formFactor() const
{
    Q_D(const Containment);
    return d->formFactor;
}

void Containment::setFormFactor(Types::FormFactor formFactor)
{
    Q_D(Containment);

    if (d->formFactor != formFactor) {
        d->formFactor = formFactor;
        Q_EMIT formFactorChanged(formFactor);
    }
}

Hawaii::Shell::Types::Location Containment::location() const
{
    Q_D(const Containment);
    return d->location;
}

void Containment::setLocation(Types::Location location)
{
    Q_D(Containment);

    if (d->location != location) {
        d->location = location;

        switch (d->location) {
        case Types::LeftEdge:
            setFormFactor(Types::Vertical);
            break;
        case Types::TopEdge:
            setFormFactor(Types::Horizontal);
            break;
        case Types::RightEdge:
            setFormFactor(Types::Vertical);
            break;
        case Types::BottomEdge:
            setFormFactor(Types::Horizontal);
            break;
        default:
            break;
        }

        Q_EMIT locationChanged(location);
    }
}

} // namespace Shell

} // namespace Hawaii

#include "moc_containment.cpp"
