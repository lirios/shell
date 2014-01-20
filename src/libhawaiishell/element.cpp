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

namespace Hawaii {

namespace Shell {

/*
 * ElementPrivate
 */

class ElementPrivate
{
public:
    ElementPrivate();

    bool busy;
    qreal minimumWidth;
    qreal maximumWidth;
    qreal minimumHeight;
    qreal maximumHeight;
    bool fillWidth;
    bool fillHeight;
};

ElementPrivate::ElementPrivate()
    : busy(false)
    , minimumWidth(0)
    , maximumWidth(10000)
    , minimumHeight(0)
    , maximumHeight(10000)
    , fillWidth(false)
    , fillHeight(false)
{
}

/*
 * Element
 */

Element::Element(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new ElementPrivate())
{
}

Element::~Element()
{
    delete d_ptr;
}

bool Element::isBusy() const
{
    Q_D(const Element);
    return d->busy;
}

void Element::setBusy(bool value)
{
    Q_D(Element);

    if (d->busy != value) {
        d->busy = value;
        Q_EMIT busyChanged();
    }
}

qreal Element::minimumWidth() const
{
    Q_D(const Element);
    return d->minimumWidth;
}

void Element::setMinimumWidth(qreal value)
{
    Q_D(Element);

    if (d->minimumWidth != value) {
        d->minimumWidth = value;
        Q_EMIT minimumWidthChanged();
    }
}

qreal Element::maximumWidth() const
{
    Q_D(const Element);
    return d->maximumWidth;
}

void Element::setMaximumWidth(qreal value)
{
    Q_D(Element);

    if (d->maximumWidth != value) {
        d->maximumWidth = value;
        Q_EMIT maximumWidthChanged();
    }
}

qreal Element::minimumHeight() const
{
    Q_D(const Element);
    return d->minimumHeight;
}

void Element::setMinimumHeight(qreal value)
{
    Q_D(Element);

    if (d->minimumHeight != value) {
        d->minimumHeight = value;
        Q_EMIT minimumHeightChanged();
    }
}

qreal Element::maximumHeight() const
{
    Q_D(const Element);
    return d->maximumHeight;
}

void Element::setMaximumHeight(qreal value)
{
    Q_D(Element);

    if (d->maximumHeight != value) {
        d->maximumHeight = value;
        Q_EMIT maximumHeightChanged();
    }
}

bool Element::fillWidth() const
{
    Q_D(const Element);
    return d->fillWidth;
}

void Element::setFillWidth(bool value)
{
    Q_D(Element);

    if (d->fillWidth != value) {
        d->fillWidth = value;
        Q_EMIT fillWidthChanged();
    }
}

bool Element::fillHeight() const
{
    Q_D(const Element);
    return d->fillHeight;
}

void Element::setFillHeight(bool value)
{
    Q_D(Element);

    if (d->fillHeight != value) {
        d->fillHeight = value;
        Q_EMIT fillHeightChanged();
    }
}

} // namespace Shell

} // namespace Hawaii

#include "moc_element.cpp"
