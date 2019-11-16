/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

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
