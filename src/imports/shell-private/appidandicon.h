// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPIDANDICON_H
#define APPIDANDICON_H

#include <QObject>

class AppIdAndIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString canonicalAppId READ canonicalAppId NOTIFY appIdChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
public:
    explicit AppIdAndIcon(QObject *parent = nullptr);

    QString appId() const;
    void setAppId(const QString &appId);

    QString canonicalAppId() const;
    QString iconName() const;

Q_SIGNALS:
    void appIdChanged();
    void iconNameChanged();

private:
    QString m_appId;
    QString m_canonicalAppId;
    QString m_iconName;

    void updateIconName();
};

#endif // APPIDANDICON_H
