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
#include <QtQuick/QQuickItem>

#include <QtConfiguration/QConfiguration>

#include "containment.h"
#include "pluginloader.h"
#include "qmlobject.h"

namespace Hawaii {

namespace Shell {

/*
 * ContainmentPrivate
 */

class ContainmentPrivate
{
public:
    ContainmentPrivate();

    void loadToolBox();

    QConfiguration *configuration;
    Corona *corona;
    Types::ContainmentType type;
    Types::FormFactor formFactor;
    Types::Location location;
    Package package;
    QmlObject *qmlObject;
};

ContainmentPrivate::ContainmentPrivate()
    : corona(nullptr)
    , type(Types::UnknownContainment)
    , formFactor(Types::Plane)
    , location(Types::Desktop)
{
}

void ContainmentPrivate::loadToolBox()
{
    QElapsedTimer timer;
    timer.start();

    // Load tool box
    // FIXME: For now this is hard-coded, load the tool box name from somewhere
    Package package = PluginLoader::instance()->loadPackage(
                QStringLiteral("Hawaii/Shell/ToolBox"));
    if (type == Types::DesktopContainment)
        package.setPath("org.hawaii.toolboxes.desktop");
    else
        package.setPath("org.hawaii.toolboxes.panel");
    if (!package.isValid()) {
        qWarning() << "Failed to load tool box: package is not valid";
        return;
    }

    qDebug() << "-> Loading tool box" << package.metadata().internalName();

    QObject *containmentObject = qmlObject->rootObject();
    if (containmentObject) {
        QVariantHash properties;
        properties["parent"] = QVariant::fromValue(containmentObject);

        QUrl url = QUrl::fromLocalFile(package.filePath("mainscript"));
        QObject *configButton = qmlObject->createObjectFromSource(url, properties);
        if (configButton)
            containmentObject->setProperty("configButton", QVariant::fromValue(configButton));

        qDebug() << "  Tool box" << package.metadata().internalName()
                 << "created in" << timer.elapsed() << "ms";
    } else {
        qWarning() << "  Tool box" << package.metadata().internalName()
                   << "not loaded because containment is not yet ready";
    }
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
    d->qmlObject = new QmlObject(this);
    d->qmlObject->setInitializationDelayed(true);

    // Save and load settings
    static int containmentId = 0;
    const QString section = QString("shell/%1/containments/containment%2")
            .arg(corona->shell())
            .arg(QString::number(containmentId++));
    d->configuration = new QConfiguration(this, section, this);
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

Hawaii::Shell::Types::ContainmentType Containment::type() const
{
    Q_D(const Containment);
    return d->type;
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

void Containment::setContextProperty(const QString &name, const QVariant &value)
{
    Q_D(Containment);
    d->qmlObject->engine()->rootContext()->setContextProperty(name, value);
}

Package Containment::package() const
{
    Q_D(const Containment);
    return d->package;
}

void Containment::setPackage(const Package &package)
{
    Q_D(Containment);

    // Read the containment type
    QString containmentType = package.metadata().property(
                QStringLiteral("X-Hawaii-ContainmentType")).toString();
    if (containmentType == QStringLiteral("Desktop"))
        d->type = Types::DesktopContainment;
    else if (containmentType == QStringLiteral("Panel"))
        d->type = Types::PanelContainment;
    else
        qFatal("Containment \"%s\" has an invalid type \"%s\", aborting...",
               qPrintable(package.metadata().internalName()),
               qPrintable(containmentType));

    // Load QML file
    d->qmlObject->setSource(QUrl::fromLocalFile(package.filePath("mainscript")));
    if (!d->qmlObject->engine() || !d->qmlObject->engine()->rootContext() ||
            !d->qmlObject->engine()->rootContext()->isValid() ||
            d->qmlObject->mainComponent()->isError()) {
        QString errorMsg;
        for (QQmlError error: d->qmlObject->mainComponent()->errors())
            errorMsg += error.toString() + QStringLiteral("\n");
        errorMsg = tr("Error loading QML file for containment: %1").arg(errorMsg);
        qWarning() << qPrintable(errorMsg);

        // Load the element error component from corona's package
        d->qmlObject->setSource(QUrl::fromLocalFile(corona()->package().filePath("elementerror")));
        d->qmlObject->completeInitialization();

        // If even the element error component cannot be loaded this is a lost cause
        if (d->qmlObject->mainComponent()->isError())
            return;

        // Element error component was loaded, set error message
        d->qmlObject->rootObject()->setProperty("errorMessage", errorMsg);
    }

    // Load is complete
    d->qmlObject->completeInitialization();

    // Keep track of the root item
    QQuickItem *rootItem = qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
    if (rootItem)
        setProperty("item", QVariant::fromValue(rootItem));

    // Load tool box
    d->loadToolBox();

    // Assign the package and notify observers
    d->package = package;
    Q_EMIT packageChanged(package);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_containment.cpp"
