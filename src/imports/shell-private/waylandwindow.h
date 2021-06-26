// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAYLANDWINDOW_H
#define WAYLANDWINDOW_H

#include <QObject>

class WaylandWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
public:
    explicit WaylandWindow(QObject *parent = nullptr);

    QString appId() const;
    void setAppId(const QString &appId);

    QString iconName() const;

Q_SIGNALS:
    void appIdChanged();
    void iconNameChanged();

private:
    QString m_appId;
    QString m_iconName;

    void updateIconName();
};

#endif // WAYLANDWINDOW_H
