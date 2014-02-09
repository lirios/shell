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

#include "configwindow_p.h"
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
    ~QuickViewPrivate();

    Corona *corona;
    QPointer<Containment> containment;
    QPointer<ConfigWindow> configWindow;
    QmlObject *configViewObject;
    bool immutable;
    bool configuring;

    void initialize();

    bool loadConfigurationComponent();

protected:
    QuickView *const q_ptr;
};

QuickViewPrivate::QuickViewPrivate(QuickView *view)
    : configWindow(nullptr)
    , immutable(false)
    , configuring(false)
    , q_ptr(view)
{
    configViewObject = new QmlObject();
    configViewObject->setInitializationDelayed(true);
}

QuickViewPrivate::~QuickViewPrivate()
{
    configViewObject->deleteLater();
    if (!configWindow.isNull())
        configWindow.data()->deleteLater();
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

bool QuickViewPrivate::loadConfigurationComponent()
{
    Q_Q(QuickView);

    QUrl url = QUrl::fromLocalFile(containment.data()->package().filePath("configuration"));
    configViewObject->setSource(url);

    if (!configViewObject->engine() ||
            !configViewObject->engine()->rootContext() ||
            !configViewObject->engine()->rootContext()->isValid() ||
            configViewObject->mainComponent()->isError()) {
        QString errorMsg;
        for (QQmlError error: configViewObject->mainComponent()->errors())
            errorMsg += error.toString() + QStringLiteral("\n");
        errorMsg = q->tr("Error loading configuration view: %1").arg(errorMsg);
        qWarning() << qPrintable(errorMsg);

        // Load the element error component from corona's package
        configViewObject->setSource(QUrl::fromLocalFile(corona->package().filePath("elementerror")));
        configViewObject->completeInitialization();

        // If even the element error component cannot be loaded this is a lost cause
        if (configViewObject->mainComponent()->isError())
            return false;

        // Element error component was loaded, set error message
        configViewObject->rootObject()->setProperty("errorMessage", errorMsg);
    }

    // Load is complete
    configViewObject->completeInitialization();

    return true;
}

/*
 * QuickView
 */

QuickView::QuickView(Corona *corona, QWindow *parent)
    : QQuickView(corona->engine(), parent)
    , d_ptr(new QuickViewPrivate(this))
{
    Q_D(QuickView);
    d->corona = corona;
    d->initialize();
}

QuickView::~QuickView()
{
    delete d_ptr;
}

Corona *QuickView::corona() const
{
    Q_D(const QuickView);
    return d->corona;
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

    // Let the configuration window know about us
    d->configViewObject->engine()->rootContext()->setContextProperty(
                "hawaiiShellQuickView", QVariant::fromValue(this));

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
        // Create the configuration view if necessary
        if (value && !d->configViewObject->rootObject())
            if (!d->loadConfigurationComponent())
                return;

        // Always destroy the window
        if (!d->configWindow.isNull()) {
            d->configWindow.data()->hide();
            d->configWindow.data()->deleteLater();
        }

        // Show the configuration window
        if (value) {
            QQuickItem *item = qobject_cast<QQuickItem *>(d->configViewObject->rootObject());
            if (!item) {
                qWarning() << "Configuration view is not an Item!";
                return;
            }

            d->configWindow = new ConfigWindow(this);
            d->configWindow.data()->setTransientParent(this);
            d->configWindow.data()->setContent(item);
            d->configWindow.data()->setPosition(d->configWindow.data()->mapToGlobal(QPoint(0, -100)));
            d->configWindow.data()->show();
        }

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

} // Shell namespace

} // Hawaii namespace

#include "moc_quickview.cpp"
