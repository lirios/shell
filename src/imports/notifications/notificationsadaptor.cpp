// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtCore/QAtomicInt>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlPropertyMap>

#include "notificationsadaptor.h"
#include "notificationsimagesstorage.h"
#include "notificationsimageutils.h"
#include "notificationsserver.h"

/*
 * Latest specifications:
 * http://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html
 */

const QString serviceName(QStringLiteral("org.freedesktop.Notifications"));
const QString servicePath(QStringLiteral("/org/freedesktop/Notifications"));

Q_LOGGING_CATEGORY(NOTIFICATIONS, "liri.notifications")

NotificationsAdaptor::NotificationsAdaptor(NotificationsServer *parent)
    : QObject()
    , m_server(parent)
{
    // Create a seed for notifications identifiers, starting from 1
    m_idSeed = new QAtomicInt(1);

    // List of applications that will send us too many notifications
    // TODO: Add more from configuration
    m_spamApplications << QStringLiteral("Clementine") << QStringLiteral("Spotify");
}

NotificationsAdaptor::~NotificationsAdaptor()
{
    unregisterService();
    delete m_idSeed;
}

bool NotificationsAdaptor::registerService()
{
    if (m_registered)
        return true;

    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerService(serviceName)) {
        qCWarning(NOTIFICATIONS,
                  "Failed to register D-Bus service \"%s\" on session bus: \"%s\"",
                  qPrintable(serviceName),
                  qPrintable(bus.lastError().message()));
        return false;
    }

    if (!bus.registerObject(servicePath, this, QDBusConnection::ExportAllContents)) {
        qCWarning(NOTIFICATIONS,
                  "Failed to register D-Bus object \"%s\" on session bus: \"%s\"",
                  qPrintable(servicePath),
                  qPrintable(bus.lastError().message()));
        return false;
    }

    m_registered = true;

    return true;
}

void NotificationsAdaptor::unregisterService()
{
    if (m_registered) {
        QDBusConnection bus = QDBusConnection::sessionBus();
        bus.unregisterObject(servicePath);
        bus.unregisterService(serviceName);
    }
}

uint NotificationsAdaptor::Notify(const QString &appName, uint replacesId,
                                  const QString &appIcon, const QString &summary,
                                  const QString &body,
                                  const QStringList &actions,
                                  const QVariantMap &hints, int timeout)
{
    // Don't create a new notification if it comes from the same source
    const auto notifications = m_notifications.values();
    if (notifications.contains(appName + summary))
        replacesId = m_notifications.key(appName + summary);

    // Calculate identifier
    uint id = replacesId > 0 ? replacesId : nextId();

    // Some applications (mostly media players) will send too many notifications,
    // for them we want to replace an older notification (if applicable)
    if (m_spamApplications.contains(appName)) {
        if (m_replaceableNotifications.contains(appName))
            id = m_replaceableNotifications.value(appName);
        else
            m_replaceableNotifications.insert(appName, id);
    }

    qCDebug(NOTIFICATIONS)
            << "Notification:"
            << "ID =" << id
            << "replaced =" << (replacesId == id)
            << appName << appIcon << summary << body
            << actions << hints << timeout;

    // Rectify appName if needed
    QString realAppName = appName;
    if (realAppName.isEmpty())
        realAppName = tr("Unknown Application");

    // Recalculate timeout depending on text, but ensure it last at least 5s
    bool isPersistent = timeout == 0;
    const int averageWordLength = 6;
    const int wordPerMinute = 250;
    int totalLength = summary.length() + body.length();
    timeout = 2000 + qMax(60000 * totalLength / averageWordLength / wordPerMinute, 3000);

    // Notification image
    NotificationImage *notificationImage = new NotificationImage();
    notificationImage->iconName = appIcon;

    // Fetch the image hint (we also support the obsolete icon_data hint which
    // is still used by applications compatible with the specification version
    if (hints.contains(QStringLiteral("image_data")))
        notificationImage->image.convertFromImage(decodeImageHint(hints[QStringLiteral("image_data")].value<QDBusArgument>()));
    else if (hints.contains(QStringLiteral("image-data")))
        notificationImage->image.convertFromImage(decodeImageHint(hints[QStringLiteral("image-data")].value<QDBusArgument>()));
    else if (hints.contains(QStringLiteral("image_path")))
        notificationImage->image = QPixmap(hints[QStringLiteral("image_path")].toString());
    else if (hints.contains(QStringLiteral("image-path")))
        notificationImage->image = QPixmap(hints[QStringLiteral("image-path")].toString());
    else if (hints.contains(QStringLiteral("icon_data")))
        notificationImage->image.convertFromImage(decodeImageHint(hints[QStringLiteral("icon_data")].value<QDBusArgument>()));

    // Retrieve icon from desktop entry, if any
    if (hints.contains(QStringLiteral("desktop-entry"))) {
        QString fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                                  hints[QStringLiteral("desktop-entry")].toString());
        QSettings desktopEntry(fileName, QSettings::IniFormat);
        notificationImage->entryIconName = desktopEntry.value(QStringLiteral("Icon"), appIcon).toString();
    }

    // Has icon?
    bool hasIcon = !notificationImage->image.isNull() ||
            !notificationImage->iconName.isEmpty() ||
            !notificationImage->entryIconName.isEmpty();

    // Store image
    NotificationsImagesStorage::instance()->add(id, notificationImage);

    // Create actions property map
    QVariantList actionsList;
    for (int i = 0; i < actions.size(); i += 2) {
        QVariantMap actionsMap;
        actionsMap[QStringLiteral("id")] = actions.at(i);
        actionsMap[QStringLiteral("text")] = actions.at(i + 1);
        actionsList.append(actionsMap);
    }

    // Create notification
    NotificationItem *item = nullptr;
    if (replacesId > 0) {
        for (int i = 0; i < m_items.size(); i++) {
            auto *currentItem = m_items.at(i);
            if (currentItem->notificationId == replacesId) {
                item = currentItem;
                break;
            }
        }
    } else {
        item = new NotificationItem();
        m_notifications.insert(id, appName + summary);
        m_items.append(item);
    }
    item->notificationId = id;
    item->appName = appName;
    item->appIcon = appIcon;
    item->iconUrl = QUrl(QStringLiteral("image://notifications/%1/%2").arg(id).arg(QDateTime::currentMSecsSinceEpoch()));
    item->hasIcon = hasIcon;
    item->summary = summary;
    item->body = body;
    item->actions = actionsList;
    item->isPersistent = isPersistent;
    item->expireTimeout = timeout;
    item->hints = hints;
    if (replacesId > 0)
        Q_EMIT m_server->notificationReplaced(item);
    else
        Q_EMIT m_server->notificationAdded(item);

    return id;
}

void NotificationsAdaptor::CloseNotification(uint id)
{
    if (m_notifications.remove(id) > 0) {
        NotificationsImagesStorage::instance()->remove(id);
        Q_EMIT NotificationClosed(id, (uint)NotificationsAdaptor::CloseReasonByApplication);
        Q_EMIT m_server->notificationRemoved(id);
    }
}

QStringList NotificationsAdaptor::GetCapabilities()
{
    return QStringList()
            << QStringLiteral("body")
            << QStringLiteral("body-hyperlinks")
            << QStringLiteral("body-markup")
            << QStringLiteral("icon-static")
            << QStringLiteral("actions")
            << QStringLiteral("persistence");
}

QString NotificationsAdaptor::GetServerInformation(QString &vendor, QString &version, QString &specVersion)
{
    vendor = QStringLiteral("Liri");
    version = QStringLiteral(PROJECT_VERSION);
    specVersion = QStringLiteral("1.1");
    return QStringLiteral("Liri Shell");
}

uint NotificationsAdaptor::nextId()
{
    return (uint)m_idSeed->fetchAndAddAcquire(1);
}
