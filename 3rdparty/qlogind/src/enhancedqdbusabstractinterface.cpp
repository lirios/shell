/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "enhancedqdbusabstractinterface.h"

#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDebug>

#include <QMetaProperty>
#include <QMetaMethod>

#define DBUS_INTERFACE_PROPERTIES "org.freedesktop.DBus.Properties"

EnhancedQDBusAbstractInterface::EnhancedQDBusAbstractInterface(const QString &service, const QString &path, const char *interface, QDBusConnection connection, QObject *parent):
    QDBusAbstractInterface(service, path, interface, connection, parent),
    m_hasInitialProperties(false)
{
    connection.connect(service, path, QLatin1String(DBUS_INTERFACE_PROPERTIES), QLatin1String("PropertiesChanged"),
                         this, SLOT(remotePropertiesChanged(QString, QVariantMap, QStringList)));
}

bool EnhancedQDBusAbstractInterface::hasInitialProperties() const
{
    return m_hasInitialProperties;
}

void EnhancedQDBusAbstractInterface::fetchInitialProperties()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(),
                                                    QLatin1String(DBUS_INTERFACE_PROPERTIES),
                                                    QLatin1String("GetAll"));

    msg.setArguments(QList<QVariant>() << interface());

    QDBusPendingCall pendingGetAll = connection().asyncCall(msg, timeout());
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingGetAll, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &EnhancedQDBusAbstractInterface::hanldleInitialProperties);
}

QVariant EnhancedQDBusAbstractInterface::property(const char* property) const
{
    if (!m_hasInitialProperties) {
        qWarning() << "Trying to access property '" << qPrintable(property) << "' before" << interface() << "is loaded";
    }

    return m_properties.value(property);
}

void EnhancedQDBusAbstractInterface::hanldleInitialProperties(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;
    call->deleteLater();

    if (reply.isError()) {
        qWarning() << reply.error();
    }

    m_properties = reply.value();

    m_hasInitialProperties = true;
    emit initialPropertiesFetched();
}

void EnhancedQDBusAbstractInterface::remotePropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interface);

    qDebug() << "a property changed " << changedProperties << invalidatedProperties;

    //TODO optimise
    foreach (const QString &key, changedProperties.keys()) {
        m_properties.insert(key, changedProperties[key]);

        //notify of changes by finding the signal relevant to this property
        const int propertyIndex = metaObject()->indexOfProperty(key.toLatin1());
        if (propertyIndex >= 0) {
            const QMetaProperty p = metaObject()->property(propertyIndex);
            if (p.hasNotifySignal()) {
                p.notifySignal().invoke(this);
            }
        }
    }

    foreach (const QString &invalidatedProperty, invalidatedProperties) {
        m_properties.remove(invalidatedProperty);

        const int propertyIndex = metaObject()->indexOfProperty(invalidatedProperty.toLatin1());
        if (propertyIndex >= 0) {
            const QMetaProperty p = metaObject()->property(propertyIndex);
            if (p.hasNotifySignal()) {
                p.notifySignal().invoke(this);
            }
        }
    }
}

