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
#include <QCoreApplication>
#include <QScreen>
#include <QDBusConnection>
#include <QQmlEngine>
#include <QQmlComponent>

#include "notificationsadaptor.h"
#include "notificationsdaemon.h"
#include "notificationsimage.h"
#include "desktopshell.h"
#include "shellview.h"

/*
 * Latest specifications:
 * http://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html
 */

NotificationsDaemon *NotificationsDaemon::self = 0;

NotificationsDaemon::NotificationsDaemon()
    : QObject()
{
    // Initialize the singleton
    Q_ASSERT_X(!self, "NotificationsDaemon", "there should be only one notifications manager");
    NotificationsDaemon::self = this;

    // Create the DBus adaptor
    new NotificationsAdaptor(this);

    // Create a seed for notifications identifiers, starting from 1
    m_idSeed = new QAtomicInt(1);

    // Set default alignment
    // TODO: from settings
    m_alignment = Qt::AlignTop | Qt::AlignRight;
}

NotificationsDaemon::~NotificationsDaemon()
{
    self = 0;
    delete m_idSeed;
}

uint NotificationsDaemon::nextId()
{
    return (uint)m_idSeed->fetchAndAddAcquire(1);
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

uint NotificationsDaemon::Notify(const QString &appName, uint replacesId, const QString &_iconName,
                                 const QString &summary, const QString &body, const QStringList &actions,
                                 const QVariantMap &hints, int timeout)
{
    qDebug() << "New notification:" << appName << replacesId << _iconName << summary << body << actions << hints << timeout;

    QString iconName = _iconName;

    // Close the notification we want to replace, if any
    if (replacesId > 0)
        CloseNotification(replacesId);

    // Do we need to change the default coordinates?
    bool changeCoords = (hints.contains("x") && hints.contains("y"));

    // Find an existing notification item
    QQuickItem *item = findNotificationItem(appName, summary);
    if (item && !body.isEmpty() && !changeCoords) {
        qDebug() << "Found already created window, id" << item->property("identifier").toInt();

        QMetaObject::invokeMethod(item, "appendToBody", Q_ARG(QVariant, body), Q_ARG(QVariant, timeout));
        QMetaObject::invokeMethod(item, "start");

        return item->property("identifier").toUInt();
    }

    // Fetch the image hint
    QImage image;
    if (hints.contains(QLatin1String("image_data")) || hints.contains(QLatin1String("icon_data"))) {
        QDBusArgument arg;

        if (hints.contains(QLatin1String("image_data")))
            arg = hints["image_data"].value<QDBusArgument>();
        else if (hints.contains(QLatin1String("icon_data"))) {
            /*
             * This hint was in use in version 1.0 of the specification and
             * got replaced by image_data in version 1.1, we support it
             * for applications that still rely on it.  Applications using
             * our API won't never use it, but supporting it makes outdated
             * applications working!
             */
            arg = hints["icon_data"].value<QDBusArgument>();
        }

        // Read image information from the argument
        int width, height, stride, hasAlpha, bitsPerSample, channels;
        QByteArray pixels;
        arg.beginStructure();
        arg >> width >> height >> stride >> hasAlpha >> bitsPerSample >> channels >> pixels;
        arg.endStructure();

        image = decodeImageHint(width, height, stride, hasAlpha, bitsPerSample, channels, pixels);
    } else if (hints.contains("image_path")) {
        iconName = findImageFromPath(hints["image_path"].toString());
    }

    // Unless an URL is provided use an icon from the theme
    if (!iconName.startsWith("file:"))
        iconName = "image://desktoptheme/" + iconName;

    // Create the notification item
    item = createNotificationItem(appName, iconName, summary, body, image, timeout);
    if (!item)
        return -1;
    qDebug() << "Created a new notification, id" << item->property("identifier").toInt();
    connect(item, SIGNAL(closed(int)), this, SLOT(notificationExpired(int)));
    m_items << item;

    // Notification item default position
    QPointF pos = position(item);

    // Force coordinates
    if (changeCoords) {
        pos.setX(hints["x"].value<QVariant>().toReal());
        pos.setY(hints["y"].value<QVariant>().toReal());
    }

    // Set notification item position
    item->setPosition(pos);

    // Show the notification view
    if (m_items.size() == 1)
        QMetaObject::invokeMethod(item, "start");

    return item->property("identifier").toUInt();
}

void NotificationsDaemon::CloseNotification(uint id)
{
    slotNotificationsItemClosed(id, (uint)CloseReasonByApplication);
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
    vendor = QStringLiteral("Maui Project");
    version = QCoreApplication::instance()->applicationVersion();
    specVersion = QStringLiteral("1.1");

    return QCoreApplication::instance()->applicationName();
}

QQuickItem *NotificationsDaemon::createNotificationItem(const QString &appName,
                                                        const QString &iconName,
                                                        const QString &summary,
                                                        const QString &body,
                                                        const QImage &image,
                                                        int timeout)
{
    ShellView *view = DesktopShell::instance()->shellView();

    // Create the component
    QQmlComponent component(view->engine(), QUrl("qrc:///qml/NotificationItem.qml"),
                            view->rootObject());

    // Create the object
    QObject *object = component.create(view->rootContext());
    if (!object) {
        qWarning() << "Couldn't create a NotificationItem object";
        return 0;
    }

    // Cast it to a QQuickItem
    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    if (!item) {
        qWarning() << "Couldn't cast NotificationItem object to QQuickItem";
        return 0;
    }

    // Set all the properties
    item->setParentItem(view->rootObject());
    item->setProperty("identifier", nextId());
    item->setProperty("appName", appName);
    item->setProperty("iconName", iconName);
    item->setProperty("summary", summary);
    item->setProperty("body", body);
    item->setProperty("image", image);
    item->setProperty("timeout", timeout);

    return item;
}

QQuickItem *NotificationsDaemon::findNotificationItem(const QString &appName, const QString &summary)
{
    for (int i = 0; i < m_items.size(); i++) {
        QQuickItem *item = m_items.at(i);

        if (item->property("appName") == appName || item->property("summary") == summary)
            return item;
    }

    return 0;
}

QString NotificationsDaemon::findImageFromPath(const QString &imagePath)
{
    if (imagePath.startsWith("file:")) {
        QUrl url(imagePath);
        return url.toLocalFile();
    }

    return imagePath;
}

Qt::Alignment NotificationsDaemon::alignment() const
{
    // TODO: Read from configuration
    return m_alignment;
}

QPointF NotificationsDaemon::position(QQuickItem *item)
{
    if (!item)
        return QPoint(0, 0);

    ShellView *view = DesktopShell::instance()->shellView();
    QQuickItem *lastItem = m_items.last();

    QSize sh(item->width(), item->height());
    QRectF rect = view->availableGeometry();

    qreal left, top;

    if (alignment() & Qt::AlignTop)
        top = rect.top() + lastItem->position().y();
    else if (alignment() & Qt::AlignVCenter)
        top = rect.top() + lastItem->position().y() + (rect.height() - sh.height()) / 2;
    else
        top = rect.bottom() - lastItem->position().y() - sh.height();

    if (alignment() & Qt::AlignLeft)
        left = rect.left();
    else if (alignment() & Qt::AlignHCenter)
        left = rect.left() + (rect.width() - sh.width()) / 2;
    else
        left = rect.right() - sh.width();

    return QPointF(left, top);
}

void NotificationsDaemon::notificationExpired(int id)
{
    slotNotificationsItemClosed((uint)id, (uint)CloseReasonExpired);
}

void NotificationsDaemon::slotNotificationsItemClosed(uint id, uint reason)
{
    emit NotificationClosed(id, reason);

    if (m_items.isEmpty()) {
        qWarning() << "The list of notifications should not be empty!";
        return;
    }

    QQuickItem *item = m_items.takeFirst();
    item->deleteLater();

    if (!m_items.isEmpty()) {
        item = m_items.first();

        QPointF pt = item->position();
        if (pt.isNull())
            pt = position(item);

        item->setPosition(pt);
        QMetaObject::invokeMethod(item, "start");
    }
}

#include "moc_notificationsdaemon.cpp"
