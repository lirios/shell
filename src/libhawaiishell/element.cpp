/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "element.h"
#include "package.h"
#include "pluginloader.h"

namespace Hawaii {

namespace Shell {

/*
 * ElementPrivate
 */

class ElementPrivate
{
public:
    ElementPrivate(Element *self);

    uint elementId;
    Containment *containment;
    Package package;
    QString errorString;

    static uint s_maxElementId;

protected:
    Q_DECLARE_PUBLIC(Element)
    Element *q_ptr;
};

uint ElementPrivate::s_maxElementId = 0;

ElementPrivate::ElementPrivate(Element *self)
    : package(nullptr)
    , q_ptr(self)
{
    elementId = ++s_maxElementId;
}

/*
 * Element
 */

Element::Element(const QString &name, Containment *containment, QObject *parent)
    : QObject(parent)
    , d_ptr(new ElementPrivate(this))
{
    Q_D(Element);
    d->containment = containment;

    d->package = PluginLoader::instance()->loadPackage(
                PluginLoader::ElementPlugin);
    d->package.setPath(name);
    Q_ASSERT(d->package.isValid());
    Q_ASSERT(!d->package.path().isEmpty());
}

Element::~Element()
{
    delete d_ptr;
}

uint Element::elementId() const
{
    Q_D(const Element);
    return d->elementId;
}

Package Element::package() const
{
    Q_D(const Element);
    return d->package;
}

Containment *Element::containment() const
{
    Q_D(const Element);
    return d->containment;
}

QString Element::errorString() const
{
    Q_D(const Element);
    return d->errorString;
}

void Element::setErrorString(const QString &error)
{
    Q_D(Element);

    if (d->errorString != error) {
        d->errorString = error;
        Q_EMIT errorStringChanged();
    }
}

} // namespace Shell

} // namespace Hawaii

#include "moc_element.cpp"
