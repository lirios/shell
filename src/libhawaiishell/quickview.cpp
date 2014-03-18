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

#include <QtCore/QPointer>
#include <QtGui/QScreen>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>

#include "containment.h"
#include "qmlobject.h"
#include "quickview.h"

namespace Hawaii {

namespace Shell {

/*
 * QuickViewPrivate
 */

class QuickViewPrivate
{
    Q_DECLARE_PUBLIC(QuickView)
public:
    QuickViewPrivate(QuickView *view);

    Mantle *mantle;
    QPointer<Containment> containment;
    bool immutable;
    bool configuring;

    void initialize();

protected:
    QuickView *const q_ptr;
};

QuickViewPrivate::QuickViewPrivate(QuickView *view)
    : immutable(false)
    , configuring(false)
    , q_ptr(view)
{
}

void QuickViewPrivate::initialize()
{
    Q_Q(QuickView);

    // Transparent color
    q->setColor(Qt::transparent);

    // Set custom window type
    q->setFlags(Qt::BypassWindowManagerHint);

    // Create platform window
    q->create();

    // Emit a signal when the screen geometry is changed
    q->connect(q->screen(), &QScreen::geometryChanged,
               q, &QuickView::screenGeometryChanged);

    // View is sized by the root object
    q->setResizeMode(QQuickView::SizeViewToRootObject);
}

/*
 * QuickView
 */

QuickView::QuickView(Mantle *mantle, QWindow *parent)
    : QQuickView(mantle->engine(), parent)
    , d_ptr(new QuickViewPrivate(this))
{
    Q_D(QuickView);
    d->mantle = mantle;
    d->initialize();
}

QuickView::~QuickView()
{
    delete d_ptr;
}

Mantle *QuickView::mantle() const
{
    Q_D(const QuickView);
    return d->mantle;
}

Containment *QuickView::containment() const
{
    Q_D(const QuickView);
    return d->containment.data();
}

void QuickView::setContainment(Containment *containment)
{
    Q_D(QuickView);

    if (d->containment.data() == containment)
        return;

    Types::FormFactor oldFormFactor = formFactor();
    Types::Location oldLocation = location();

    if (d->containment) {
        disconnect(d->containment.data(), nullptr, this, nullptr);

        QObject *oldItem = d->containment.data()->property("item").value<QObject *>();
        if (oldItem)
            oldItem->setParent(d->containment.data());
    }

    d->containment = containment;

    if (oldFormFactor != formFactor())
        Q_EMIT formFactorChanged(formFactor());
    if (oldLocation != location())
        Q_EMIT locationChanged(location());

    Q_EMIT containmentChanged();

    if (containment) {
        connect(containment, &Containment::locationChanged,
                this, &QuickView::locationChanged);
        connect(containment, &Containment::formFactorChanged,
                this, &QuickView::formFactorChanged);

        QQuickItem *item = qobject_cast<QQuickItem *>(d->containment.data()->property("item").value<QObject *>());
        if (item) {
            item->setProperty("width", width());
            item->setProperty("height", height());
            item->setParentItem(rootObject());

            if (rootObject())
                rootObject()->setProperty("containment", QVariant::fromValue(item));
        }
    }
}

bool QuickView::isImmutable() const
{
    Q_D(const QuickView);
    return d->immutable;
}

void QuickView::setImmutable(bool value)
{
    Q_D(QuickView);

    if (d->immutable != value) {
        d->immutable = value;
        Q_EMIT immutableChanged(value);
    }
}

bool QuickView::isConfiguring() const
{
    Q_D(const QuickView);
    return d->configuring;
}

void QuickView::setConfiguring(bool value)
{
    Q_D(QuickView);

    if (d->configuring != value) {
        // Views inheriting QuickView will have to override these methods
        // implementing their window setup code
        if (value)
            showConfigurationWindow();
        else
            hideConfigurationWindow();

        // Now we can actually change the property
        d->configuring = value;
        Q_EMIT configuringChanged(value);
    }
}

Hawaii::Shell::Types::FormFactor QuickView::formFactor() const
{
    Q_D(const QuickView);

    if (!d->containment)
        return Types::Plane;
    return d->containment.data()->formFactor();
}

void QuickView::setFormFactor(Types::FormFactor value)
{
    Q_D(QuickView);

    if (formFactor() != value)
        d->containment.data()->setFormFactor(value);
}

Hawaii::Shell::Types::Location QuickView::location() const
{
    Q_D(const QuickView);

    if (!d->containment)
        return Types::Desktop;
    return d->containment.data()->location();
}

void QuickView::setLocation(Types::Location value)
{
    Q_D(QuickView);

    if (location() != value)
        d->containment.data()->setLocation(value);
}

QRectF QuickView::screenGeometry() const
{
    return screen()->geometry();
}

void QuickView::showConfigurationWindow()
{
}

void QuickView::hideConfigurationWindow()
{
}

} // Shell namespace

} // Hawaii namespace

#include "moc_quickview.cpp"
