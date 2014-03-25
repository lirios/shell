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

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlProperty>

#include "containment.h"
#include "containmentitem.h"
#include "elementitem.h"
#include "package.h"
#include "qmlobject.h"

namespace Hawaii {

namespace Shell {

/*
 * ContainmentItemPrivate
 */

class ContainmentItemPrivate
{
public:
    ContainmentItemPrivate(ContainmentItem *self);

    Containment *containment;
    QmlObject *qmlObject;

    void _q_elementAdded(Element *element);
    void _q_packageChanged();

    static QHash<QObject *, ContainmentItem *> s_rootObjects;

protected:
    Q_DECLARE_PUBLIC(ContainmentItem)
    ContainmentItem *q_ptr;
};

QHash<QObject *, ContainmentItem *> ContainmentItemPrivate::s_rootObjects = QHash<QObject *, ContainmentItem *>();

ContainmentItemPrivate::ContainmentItemPrivate(ContainmentItem *self)
    : containment(nullptr)
    , qmlObject(nullptr)
    , q_ptr(self)
{
}

void ContainmentItemPrivate::_q_elementAdded(Element *element)
{
    Q_Q(ContainmentItem);

    ElementItem *elementItem = new ElementItem(element, q);
    elementItem->initialize();
    Q_EMIT q->elementAdded(elementItem);
}

void ContainmentItemPrivate::_q_packageChanged()
{
    Q_Q(ContainmentItem);

    // Delete the QML object
    qmlObject->deleteLater();

    // Recreate the QML object so that q->initialize() will
    // load the code from the new package
    qmlObject = new QmlObject(q);
    qmlObject->setInitializationDelayed(true);

    // Load the QML object
    q->initialize();
}

/*
 * ContainmentItem
 */

ContainmentItem::ContainmentItem(Containment *containment, QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new ContainmentItemPrivate(this))
{
    Q_D(ContainmentItem);
    d->containment = containment;
    d->qmlObject = new QmlObject(this);
    d->qmlObject->setInitializationDelayed(true);

    // Relation between containment and its graphical representation
    containment->setProperty("_graphicObject", QVariant::fromValue(this));
    setProperty("_internalObject", QVariant::fromValue(containment));

    // Emit signals when the containment changes
    connect(containment, &Containment::formFactorChanged,
            this, &ContainmentItem::formFactorChanged);
    connect(containment, &Containment::locationChanged,
            this, &ContainmentItem::locationChanged);
    connect(containment, &Containment::immutableChanged,
            this, &ContainmentItem::immutableChanged);
    connect(containment, &Containment::configuringChanged,
            this, &ContainmentItem::configuringChanged);

    // Handle elements
    connect(containment, SIGNAL(elementAdded(Hawaii::Shell::Element*)),
            this, SLOT(_q_elementAdded(Hawaii::Shell::Element*)));

    // Reinizialize this item when the package is changed
    connect(containment, SIGNAL(packageChanged(Package)),
            this, SLOT(_q_packageChanged()));
}

ContainmentItem::~ContainmentItem()
{
    Q_D(ContainmentItem);
    ContainmentItemPrivate::s_rootObjects.remove(d->qmlObject->engine());
}

Containment *ContainmentItem::containment() const
{
    Q_D(const ContainmentItem);
    return d->containment;
}

QQuickItem *ContainmentItem::rootObject() const
{
    Q_D(const ContainmentItem);
    return qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
}

Hawaii::Shell::Types::ContainmentType ContainmentItem::type() const
{
    Q_D(const ContainmentItem);
    return d->containment->type();
}

Hawaii::Shell::Types::FormFactor ContainmentItem::formFactor() const
{
    Q_D(const ContainmentItem);
    return d->containment->formFactor();
}

void ContainmentItem::setFormFactor(Types::FormFactor formFactor)
{
    Q_D(ContainmentItem);
    d->containment->setFormFactor(formFactor);
}

Hawaii::Shell::Types::Location ContainmentItem::location() const
{
    Q_D(const ContainmentItem);
    return d->containment->location();
}

void ContainmentItem::setLocation(Types::Location location)
{
    Q_D(ContainmentItem);
    d->containment->setLocation(location);
}

bool ContainmentItem::isImmutable() const
{
    Q_D(const ContainmentItem);
    return d->containment->isImmutable();
}

void ContainmentItem::setImmutable(bool value)
{
    Q_D(ContainmentItem);
    d->containment->setImmutable(value);
}

bool ContainmentItem::isConfiguring() const
{
    Q_D(const ContainmentItem);
    return d->containment->isConfiguring();
}

void ContainmentItem::setConfiguring(bool value)
{
    Q_D(ContainmentItem);
    d->containment->setConfiguring(value);
}

void ContainmentItem::initialize()
{
    Q_D(ContainmentItem);

    if (ContainmentItemPrivate::s_rootObjects.contains(d->qmlObject->engine()))
        return;

    ContainmentItemPrivate::s_rootObjects[d->qmlObject->engine()] = this;

    Q_ASSERT(d->containment);

    QElapsedTimer timer;
    timer.start();

    qDebug() << "-> Loading containment" << d->containment->package().metadata().internalName();

    // Load QML file
    QString url = d->containment->package().filePath("mainscript");
    d->qmlObject->setSource(QUrl::fromLocalFile(url));
    if (!d->qmlObject->engine() ||
            !d->qmlObject->engine()->rootContext() ||
            !d->qmlObject->engine()->rootContext()->isValid() ||
            d->qmlObject->mainComponent()->isError()) {
        QStringList errorMsgs;
        for (QQmlError error: d->qmlObject->mainComponent()->errors())
            errorMsgs.append(error.toString());
        QString errorMsg = tr("Error loading QML file: %1").arg(errorMsgs.join('\n'));
        qWarning("  ** %s", qPrintable(errorMsg));

        // Load the element error component from mantle's package
        url = d->containment->mantle()->package().filePath("elementerror");
        d->qmlObject->setSource(QUrl::fromLocalFile(url));
        d->qmlObject->completeInitialization();

        // If even the element error component cannot be loaded this is a lost cause
        if (d->qmlObject->mainComponent()->isError())
            return;

        // Element error component was loaded, set error message
        d->qmlObject->rootObject()->setProperty("errorMessage", errorMsg);
    } else {
        d->qmlObject->completeInitialization();
    }

    // Set component parent
    QQuickItem *item = qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
    item->setParentItem(this);

    // Fill component
    QQmlExpression expression(QtQml::qmlContext(d->qmlObject->rootObject()), item, "parent");
    QQmlProperty property(item, "anchors.fill");
    property.write(expression.evaluate());

    qDebug() << "  Containment created in" << timer.elapsed() << "ms";
}

ContainmentItem *ContainmentItem::qmlAttachedProperties(QObject *object)
{
    // When the attached object is created, the root item doesn't have a parent,
    // with this we avoid creation of the attached for everybody but the root item
    if (!object->parent() && ContainmentItemPrivate::s_rootObjects.contains(QtQml::qmlEngine(object)))
        return ContainmentItemPrivate::s_rootObjects.value(QtQml::qmlEngine(object));
    return nullptr;
}

} // namespace Shell

} // namespace Hawaii

#include "moc_containmentitem.cpp"
