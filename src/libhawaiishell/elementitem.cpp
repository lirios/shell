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
#include "element.h"
#include "elementitem.h"
#include "package.h"
#include "qmlobject.h"

namespace Hawaii {

namespace Shell {

/*
 * ElementItemPrivate
 */

class ElementItemPrivate
{
public:
    ElementItemPrivate(ElementItem *self);

    Element *element;
    bool busy : 1;
    QString title;
    QString label;
    QString iconName;
    QmlObject *qmlObject;

    QWeakPointer<QObject> originalLayout;
    QWeakPointer<QObject> ourLayout;

    void setupLayoutAttached(QObject *item);
    void propagateLayoutProperty(const char *name);

    void _q_minimumWidthChanged();
    void _q_minimumHeightChanged();
    void _q_preferredWidthChanged();
    void _q_preferredHeightChanged();
    void _q_maximumWidthChanged();
    void _q_maximumHeightChanged();
    void _q_fillWidthChanged();
    void _q_fillHeightChanged();

    static QHash<QObject *, ElementItem *> s_rootObjects;

protected:
    Q_DECLARE_PUBLIC(ElementItem)
    ElementItem *q_ptr;
};

QHash<QObject *, ElementItem *> ElementItemPrivate::s_rootObjects = QHash<QObject *, ElementItem *>();

ElementItemPrivate::ElementItemPrivate(ElementItem *self)
    : element(nullptr)
    , busy(true)
    , qmlObject(nullptr)
    , q_ptr(self)
{
}

void ElementItemPrivate::setupLayoutAttached(QObject *item)
{
    // Sanity check
    if (!item)
        return;

    Q_Q(ElementItem);

    // Loop over all the item's children to find Layout properties
    QObject *layout = nullptr;
    for (QObject *child: item->children()) {
        if (child->property("minimumWidth").isValid() &&
                child->property("minimumHeight").isValid() &&
                child->property("preferredWidth").isValid() &&
                child->property("preferredHeight").isValid() &&
                child->property("maximumWidth").isValid() &&
                child->property("maximumHeight").isValid() &&
                child->property("fillWidth").isValid() &&
                child->property("fillHeight").isValid()) {
            layout = child;
            break;
        }
    }
    //qDebug() << "Element layout:" << layout;

    // We can't continue without a Layout attached object
    if (!layout)
        return;

    // Our ElementItem doesn't have a Layout attached object, so in
    // order to create one we have to reference a Layout property
    // with this hack
    QQmlProperty p(q, "Layout.fillWidth", QtQml::qmlContext(qmlObject->rootObject()));
    Q_UNUSED(p);

    // Now let's find our Layout attached object
    QObject *ownLayout = nullptr;
    for (QObject *child: q->children()) {
        if (child->property("minimumWidth").isValid() &&
                child->property("minimumHeight").isValid() &&
                child->property("preferredWidth").isValid() &&
                child->property("preferredHeight").isValid() &&
                child->property("maximumWidth").isValid() &&
                child->property("maximumHeight").isValid() &&
                child->property("fillWidth").isValid() &&
                child->property("fillHeight").isValid()) {
            ownLayout = child;
            break;
        }
    }
    //qDebug() << "ElementItem layout:" << ownLayout;

    // Can't continue without our Layout attached object, however this
    // shouldn't happen as we have previously referenced it
    if (!ownLayout)
        return;

    // Connect children layout to our layout
    q->connect(layout, SIGNAL(minimumWidthChanged()),
               q, SLOT(_q_minimumWidthChanged()));
    q->connect(layout, SIGNAL(minimumHeightChanged()),
               q, SLOT(_q_minimumHeightChanged()));
    q->connect(layout, SIGNAL(preferredWidthChanged()),
               q, SLOT(_q_preferredWidthChanged()));
    q->connect(layout, SIGNAL(preferredHeightChanged()),
               q, SLOT(_q_preferredHeightChanged()));
    q->connect(layout, SIGNAL(maximumWidthChanged()),
               q, SLOT(_q_maximumWidthChanged()));
    q->connect(layout, SIGNAL(maximumHeightChanged()),
               q, SLOT(_q_maximumHeightChanged()));
    q->connect(layout, SIGNAL(fillWidthChanged()),
               q, SLOT(_q_fillWidthChanged()));
    q->connect(layout, SIGNAL(fillHeightChanged()),
               q, SLOT(_q_fillHeightChanged()));

    // Save the layouts we found to use them later
    originalLayout = layout;
    ourLayout = ownLayout;

    // Propagate Layout properties
    propagateLayoutProperty("minimumWidth");
    propagateLayoutProperty("minimumHeight");
    propagateLayoutProperty("preferredWidth");
    propagateLayoutProperty("preferredHeight");
    propagateLayoutProperty("maximumWidth");
    propagateLayoutProperty("maximumHeight");
    propagateLayoutProperty("fillWidth");
    propagateLayoutProperty("fillHeight");
}

void ElementItemPrivate::propagateLayoutProperty(const char *name)
{
    // Sanity check
    if (!name || !originalLayout || !ourLayout)
        return;

    // Propagate the Layout property from the element to our wrapper
    ourLayout.data()->setProperty(name, originalLayout.data()->property(name));
}

void ElementItemPrivate::_q_minimumWidthChanged()
{
    propagateLayoutProperty("minimumWidth");
}

void ElementItemPrivate::_q_minimumHeightChanged()
{
    propagateLayoutProperty("minimumHeight");
}

void ElementItemPrivate::_q_preferredWidthChanged()
{
    propagateLayoutProperty("preferredWidth");
}

void ElementItemPrivate::_q_preferredHeightChanged()
{
    propagateLayoutProperty("preferredHeight");
}

void ElementItemPrivate::_q_maximumWidthChanged()
{
    propagateLayoutProperty("maximumWidth");
}

void ElementItemPrivate::_q_maximumHeightChanged()
{
    propagateLayoutProperty("maximumHeight");
}

void ElementItemPrivate::_q_fillWidthChanged()
{
    propagateLayoutProperty("fillWidth");
}

void ElementItemPrivate::_q_fillHeightChanged()
{
    propagateLayoutProperty("fillHeight");
}

/*
 * ElementItem
 */

ElementItem::ElementItem(Element *element, QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new ElementItemPrivate(this))
{
    Q_D(ElementItem);
    d->element = element;
    d->title = element->package().metadata().name();
    d->label = element->package().metadata().comment();
    d->iconName = element->package().metadata().iconName();
    d->qmlObject = new QmlObject(this);
    d->qmlObject->setInitializationDelayed(true);

    // Relation between element and its graphical representation
    element->setProperty("_graphicObject", QVariant::fromValue(this));
    setProperty("_internalObject", QVariant::fromValue(element));

    // Emit signals when the containment changes form factor or location
    connect(element->containment(), &Containment::formFactorChanged,
            this, &ElementItem::formFactorChanged);
    connect(element->containment(), &Containment::locationChanged,
            this, &ElementItem::locationChanged);
}

ElementItem::~ElementItem()
{
    Q_D(ElementItem);
    ElementItemPrivate::s_rootObjects.remove(d->qmlObject->engine());
}

Element *ElementItem::element() const
{
    Q_D(const ElementItem);
    return d->element;
}

uint ElementItem::id() const
{
    Q_D(const ElementItem);
    return d->element->elementId();
}

Hawaii::Shell::Types::FormFactor ElementItem::formFactor() const
{
    Q_D(const ElementItem);
    return d->element->containment()->formFactor();
}

Hawaii::Shell::Types::Location ElementItem::location() const
{
    Q_D(const ElementItem);
    return d->element->containment()->location();
}

bool ElementItem::isBusy() const
{
    Q_D(const ElementItem);
    return d->busy;
}

QString ElementItem::title() const
{
    Q_D(const ElementItem);
    return d->title;
}

void ElementItem::setTitle(const QString &title)
{
    Q_D(ElementItem);

    if (d->title != title) {
        d->title = title;
        Q_EMIT titleChanged(title);
    }
}

QString ElementItem::label() const
{
    Q_D(const ElementItem);
    return d->label;
}

void ElementItem::setLabel(const QString &label)
{
    Q_D(ElementItem);

    if (d->label != label) {
        d->label = label;
        Q_EMIT labelChanged(label);
    }
}

QString ElementItem::iconName() const
{
    Q_D(const ElementItem);
    return d->iconName;
}

void ElementItem::setIconName(const QString &iconName)
{
    Q_D(ElementItem);

    if (d->iconName != iconName) {
        d->iconName = iconName;
        Q_EMIT iconNameChanged(iconName);
    }
}

void ElementItem::setContextProperty(const QString &name, const QVariant &value)
{
    Q_D(ElementItem);
    d->qmlObject->engine()->rootContext()->setContextProperty(name, value);
}

ElementItem *ElementItem::qmlAttachedProperties(QObject *object)
{
    // When the attached object is created, the root item doesn't have a parent,
    // with this we avoid creation of the attached for everybody but the root item
    if (!object->parent() && ElementItemPrivate::s_rootObjects.contains(QtQml::qmlEngine(object)))
        return ElementItemPrivate::s_rootObjects.value(QtQml::qmlEngine(object));
    return nullptr;
}

void ElementItem::initialize()
{
    Q_D(ElementItem);

    if (ElementItemPrivate::s_rootObjects.contains(d->qmlObject->engine()))
        return;

    ElementItemPrivate::s_rootObjects[d->qmlObject->engine()] = this;

    Q_ASSERT(d->element);

    QElapsedTimer timer;
    timer.start();

    qDebug() << "-> Loading element" << d->element->package().metadata().internalName();

    QUrl url = QUrl::fromLocalFile(d->element->package().filePath("mainscript"));
    d->qmlObject->setSource(url);
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
        url = QUrl::fromLocalFile(d->element->containment()->mantle()->package().filePath("elementerror"));
        d->qmlObject->setSource(url);
        d->qmlObject->completeInitialization();

        // If even the element error component cannot be loaded this is a lost cause
        if (d->qmlObject->mainComponent()->isError())
            return;

        // Element error component was loaded, set error message
        d->qmlObject->rootObject()->setProperty("errorMessage", errorMsg);

        // Set element error string
        d->element->setErrorString(errorMsg);
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

    // Connect Layout from the real element to our's
    d->setupLayoutAttached(item);

    // Element is no longer busy
    d->busy = false;
    Q_EMIT busyChanged();

    qDebug() << "  Element created in" << timer.elapsed() << "ms";
}

void ElementItem::itemChange(ItemChange change, const ItemChangeData &value)
{
    if (change == QQuickItem::ItemSceneChange) {
        if (value.window)
            initialize();
    }

    QQuickItem::itemChange(change, value);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_elementitem.cpp"
