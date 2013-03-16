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

#ifndef NOTIFICATIONSDAEMON_H
#define NOTIFICATIONSDAEMON_H

#include <QList>
#include <QDBusArgument>
#include <QImage>
#include <QQuickItem>

class QAtomicInt;

class NotificationsDaemon : public QObject
{
    Q_OBJECT
    Q_ENUMS(CloseReason)
public:
    enum CloseReason {
        CloseReasonExpired = 1,
        CloseReasonByUser,
        CloseReasonByApplication
    };

    explicit NotificationsDaemon();
    ~NotificationsDaemon();

    static NotificationsDaemon *instance() {
        return self;
    }

    uint nextId();

    bool connectOnDBus();

    uint Notify(const QString &appName, uint replacesId, const QString &iconName,
                const QString &summary, const QString &body, const QStringList &actions,
                const QVariantMap &hints, int timeout);

    void CloseNotification(uint id);

    QStringList GetCapabilities();

    QString GetServerInformation(QString &vendor, QString &version, QString &specVersion);

signals:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &actionKey);

private:
    static NotificationsDaemon *self;

    QAtomicInt *m_idSeed;
    QList<QQuickItem *> m_items;
    Qt::Alignment m_alignment;

private:
    QQuickItem *createNotificationItem(const QString &appName,
                                       const QString &iconName,
                                       const QString &summary,
                                       const QString &body,
                                       const QImage &image,
                                       int timeout = 2000);
    QQuickItem *findNotificationItem(const QString &appName,
                                     const QString &summary);

    QString findImageFromPath(const QString &imagePath);

    Qt::Alignment alignment() const;

    QPointF position(QQuickItem *item);

private slots:
    void notificationExpired(int id);
    void slotNotificationsItemClosed(uint id, uint reason);
};

#endif // NOTIFICATIONSDAEMON_H
