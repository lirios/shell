/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QAtomicInt>
#include <QDebug>
#include <QGuiApplication>
#include <QDBusConnection>
#include <QQmlEngine>
#include <QQmlComponent>

#include <qpa/qplatformnativeinterface.h>

#include "notificationsadaptor.h"
#include "notificationsdaemon.h"
#include "notificationsimage.h"
#include "notificationwindow.h"
#include "waylandintegration.h"

/*
 * Latest specifications:
 * http://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html
 */

Q_GLOBAL_STATIC(NotificationsDaemon, s_notificationsDaemon)

NotificationsDaemon::NotificationsDaemon()
    : QObject()
{
    // Create the DBus adaptor
    new NotificationsAdaptor(this);

    // Create a seed for notifications identifiers, starting from 1
    m_idSeed = new QAtomicInt(1);
}

NotificationsDaemon::~NotificationsDaemon()
{
    delete m_idSeed;
}

NotificationsDaemon *NotificationsDaemon::instance()
{
    return s_notificationsDaemon();
}

uint NotificationsDaemon::nextId()
{
    return (uint)m_idSeed->fetchAndAddAcquire(1);
}

uint NotificationsDaemon::Notify(const QString &appName, uint replacesId,
                                 const QString &_iconName,
                                 const QString &summary, const QString &body,
                                 const QStringList &actions,
                                 const QVariantMap &hints, int timeout)
{
    qDebug() << "Notification:" << appName << replacesId << _iconName << summary << body << actions << hints << timeout;

    QString iconName = _iconName;

    // Close the notification we want to replace, if any
    if (replacesId > 0)
        CloseNotification(replacesId);

    // Do we need to change the default coordinates?
    bool changeCoords = (hints.contains("x") && hints.contains("y"));

    // Find an existing notification item
    NotificationWindow *notification = findNotification(appName, summary);
    if (notification && !body.isEmpty() && !changeCoords) {
        QMetaObject::invokeMethod(notification,
                                  "appendToBody",
                                  Q_ARG(QVariant, body),
                                  Q_ARG(QVariant, timeout));

        return notification->property("identifier").toUInt();
    }

    // Fetch the image hint (we also support the obsolete icon_data hint which
    // is still used by applications compatible with the specification version
    QImage image;
    if (hints.contains(QStringLiteral("image_data")) || hints.contains(QStringLiteral("icon_data"))) {
        if (hints.contains(QStringLiteral("image_data")))
            image = decodeImageHint(hints["image_data"].value<QDBusArgument>());
        else if (hints.contains(QStringLiteral("icon_data")))
            image = decodeImageHint(hints["icon_data"].value<QDBusArgument>());
    } else if (hints.contains(QStringLiteral("image_path"))) {
        iconName = findImageFromPath(hints["image_path"].toString());
    }

    // Unless an URL is provided use an icon from the theme
    if (!iconName.startsWith(QStringLiteral("file:")))
        iconName = QStringLiteral("image://desktoptheme/") + iconName;

    // Create the notification and put it into the queue
    notification = createNotification(appName, iconName, summary,
                                      body, image, timeout);
    if (!notification)
        return -1;
    m_notifications << notification;

    // Show the notification if nothing is on the queue
    if (m_notifications.size() == 1)
        showNotification(notification);

#if 0
    // Force coordinates
    // TODO: Need to add something to the protocol
    if (changeCoords) {
        pos.setX(hints["x"].value<QVariant>().toReal());
        pos.setY(hints["y"].value<QVariant>().toReal());
    }
#endif

    return notification->property("identifier").toUInt();
}

void NotificationsDaemon::CloseNotification(uint id)
{
    notificationClosed(id, (uint)CloseReasonByApplication);
}

QStringList NotificationsDaemon::GetCapabilities()
{
    return QStringList()
            << "body"
            << "body-hyperlinks"
            << "body-markup"
            << "icon-static";
}

QString NotificationsDaemon::GetServerInformation(QString &vendor, QString &version, QString &specVersion)
{
    vendor = QStringLiteral("Hawaii");
    version = QCoreApplication::instance()->applicationVersion();
    specVersion = QStringLiteral("1.1");

    return QCoreApplication::instance()->applicationName();
}

bool NotificationsDaemon::connectOnDBus()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerObject("/org/freedesktop/Notifications", this)) {
        qWarning() << "Couldn't register object /org/freedesktop/Notifications";
        return false;
    }

    if (!connection.registerService("org.freedesktop.Notifications")) {
        qWarning() << "Couldn't register service for org.freedesktop.Notifications";
        return false;
    }

    qDebug() << "Notifications service registered!";
    return true;
}

NotificationWindow *NotificationsDaemon::createNotification(const QString &appName,
                                                            const QString &iconName,
                                                            const QString &summary,
                                                            const QString &body,
                                                            const QImage &image,
                                                            int timeout)
{
    QQmlEngine *engine = new QQmlEngine(this);
    QQmlComponent *component = new QQmlComponent(engine, this);
    component->loadUrl(QUrl("qrc:///qml/NotificationItem.qml"));
    if (!component->isReady())
        qFatal("Failed to create a notification window: %s",
               qPrintable(component->errorString()));

    QObject *topLevel = component->create();
    NotificationWindow *window = qobject_cast<NotificationWindow *>(topLevel);
    if (!window)
        qFatal("NotificationItem's root item has to be a NotificationWindow");

    // Set all the properties
    window->setProperty("identifier", nextId());
    window->setProperty("appName", appName);
    if (image.isNull())
        window->setProperty("iconName", iconName);
    window->setProperty("summary", summary);
    window->setProperty("body", body);
    if (!image.isNull())
        window->setProperty("picture", image);
    window->setProperty("timeout", timeout);

    // Handle expiration
    connect(window, SIGNAL(closed(int)), this, SLOT(notificationExpired(int)));

    return window;
}

NotificationWindow *NotificationsDaemon::findNotification(const QString &appName, const QString &summary)
{
    for (int i = 0; i < m_notifications.size(); i++) {
        NotificationWindow *window = m_notifications.at(i);

        if (window->property("appName") == appName || window->property("summary") == summary)
            return window;
    }

    return 0;
}

void NotificationsDaemon::showNotification(NotificationWindow *notification)
{
    // Show the notification (this will start the QML timer because its
    // running property is bound to visible) and ask the compositor
    // to add the surface to the "bubbles list". We process events between
    // show and addSurfaces so that the buffer is created and the surface
    // attached, otherwise the compositor will crash
    notification->show();
    QGuiApplication::processEvents();
    notification->addSurface();
}

QString NotificationsDaemon::findImageFromPath(const QString &imagePath)
{
    if (imagePath.startsWith("file:")) {
        QUrl url(imagePath);
        return url.toLocalFile();
    }

    return imagePath;
}

void NotificationsDaemon::notificationExpired(int id)
{
    notificationClosed((uint)id, (uint)CloseReasonExpired);
}

void NotificationsDaemon::notificationClosed(uint id, uint reason)
{
    // It doesn't make sense to continue if we don't have any notification left
    if (m_notifications.isEmpty())
        return;

    // Actually close the notification
    NotificationWindow *notification = m_notifications.takeFirst();
    notification->close();
    emit NotificationClosed(id, reason);

    // Show the next notification
    if (!m_notifications.isEmpty()) {
        notification = m_notifications.first();
        showNotification(notification);
    }
}

#include "moc_notificationsdaemon.cpp"
