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

#include <QAtomicInt>
#include <QDebug>
#include <QGuiApplication>
#include <QDBusConnection>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QtQuick/QQuickItem>

#include <qpa/qplatformnativeinterface.h>

#include "notificationsadaptor.h"
#include "notificationsdaemon.h"
#include "notificationsimage.h"
#include "notificationwindow.h"
#include "notificationimageprovider.h"
#include "hawaiishell.h"

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

    // Find an existing notification item unless the application wants us to
    // replace an existing bubble with a new one
    NotificationWindow *notification = 0;
    if (replacesId == 0) {
        notification = findNotification(appName, summary);
        if (notification && !body.isEmpty() && !changeCoords) {
            QMetaObject::invokeMethod(notification,
                                      "appendToBody",
                                      Q_ARG(QVariant, body),
                                      Q_ARG(QVariant, timeout));

            return notification->property("identifier").toUInt();
        }
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
    if (!iconName.startsWith(QStringLiteral("file:")) && !iconName.isEmpty())
        iconName = QStringLiteral("image://desktoptheme/") + iconName;

    // Create the notification and put it into the queue
    notification = createNotification(replacesId, appName, iconName, summary,
                                      body, image, timeout);
    if (!notification)
        return 0;
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

NotificationWindow *NotificationsDaemon::createNotification(uint replacesId,
                                                            const QString &appName,
                                                            const QString &iconName,
                                                            const QString &summary,
                                                            const QString &body,
                                                            const QImage &image,
                                                            int timeout)
{
    QQmlEngine *engine = HawaiiShell::instance()->engine();
    QQmlComponent *component = new QQmlComponent(engine, this);
    component->loadUrl(QUrl("qrc:///qml/NotificationBubble.qml"));
    if (!component->isReady())
        qFatal("Failed to create a notification bubble: %s",
               qPrintable(component->errorString()));

    QObject *topLevel = component->create();
    QQuickItem *item = qobject_cast<QQuickItem *>(topLevel);
    if (!item)
        qFatal("Can't get notification bubble item");

    // Calculate identifier
    uint id = replacesId > 0 ? replacesId : nextId();

    // Set properties
    item->setProperty("identifier", id);
    item->setProperty("appName", appName);
    item->setProperty("summary", summary);
    item->setProperty("body", body);
    item->setProperty("expirationTimeout", timeout);

    // Set icon name
    QString icon = iconName;
    if (!image.isNull()) {
        cacheImage(QString::number(id), image);
        icon = QStringLiteral("image://notifications/") + QString::number(id);
    }
    item->setProperty("iconName", icon);

    // Handle expiration
    connect(item, SIGNAL(closed(int)), this, SLOT(notificationExpired(int)));

    // Create a window
    NotificationWindow *window = new NotificationWindow();
    window->setItem(item);
    window->setScreen(QGuiApplication::primaryScreen());
    window->setWidth(item->width());
    window->setHeight(item->height());

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
    // Show the notification and start the timer that will automatically
    // close the bubble
    notification->show();
    notification->item()->setProperty("running", true);

    // We trigger a fake resize in order to have the buffer created
    // and then syncronize and add the surface so that the compositor
    // will be able to determine the buffer size
    notification->resize(notification->size());
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // Ask the compositor to add the surface to the "bubbles list"
    notification->addSurface();
    notification->setInputRegion(QRect(0, 0, 0, 0));
}

QString NotificationsDaemon::findImageFromPath(const QString &imagePath)
{
    if (imagePath.startsWith("file:")) {
        QUrl url(imagePath);
        return url.toLocalFile();
    }

    return QStringLiteral("file://") + imagePath;
}

void NotificationsDaemon::cacheImage(const QString &key, const QImage &image)
{
    QQmlImageProviderBase *base =
            HawaiiShell::instance()->engine()->imageProvider("notifications");
    NotificationImageProvider *provider =
            static_cast<NotificationImageProvider *>(base);
    if (provider)
        provider->insertPixmap(key, QPixmap::fromImage(image));
}

void NotificationsDaemon::uncacheImage(const QString &key)
{
    QQmlImageProviderBase *base =
            HawaiiShell::instance()->engine()->imageProvider("notifications");
    NotificationImageProvider *provider =
            static_cast<NotificationImageProvider *>(base);
    if (provider)
        provider->removePixmap(key);
}

void NotificationsDaemon::notificationExpired(int id)
{
    notificationClosed((uint)id, (uint)CloseReasonExpired);
}

void NotificationsDaemon::notificationClosed(uint id, uint reason)
{
    // Close the notification, if it's asked by the application it means that
    // it wants to replace this notification with a new one so we return
    // so that Notify will create a new bubble
    for (int i = 0; i < m_notifications.size(); i++) {
        NotificationWindow *notification = m_notifications.at(i);
        uint notificationId = notification->property("identifier").toUInt();
        if (notificationId != id) {
            m_notifications.removeAt(i);
            notification->hide();
            notification->deleteLater();
            uncacheImage(QString::number(notificationId));
            emit NotificationClosed(id, reason);
            return;
        }
    }

    // Show the next notification on the queue
    if (!m_notifications.isEmpty()) {
        NotificationWindow *notification = m_notifications.takeFirst();
        showNotification(notification);
    }
}

#include "moc_notificationsdaemon.cpp"
