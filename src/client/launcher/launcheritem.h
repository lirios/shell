/****************************************************************************
 * This file is part of Hawaii Shell.
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

#ifndef LAUNCHERITEM_H
#define LAUNCHERITEM_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class LauncherItemPrivate;

class LauncherItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool urgent READ isUrgent NOTIFY urgentChanged)
    Q_PROPERTY(bool draggable READ isDraggable NOTIFY draggableChanged)
    Q_PROPERTY(bool editable READ isEditable NOTIFY editableChanged)
    Q_PROPERTY(bool removable READ isRemovable NOTIFY removableChanged)
    Q_PROPERTY(bool counterVisible READ isCounterVisible NOTIFY counterVisibleChanged)
    Q_PROPERTY(int counter READ counter NOTIFY counterChanged)
    Q_ENUMS(Type)
public:
    enum Type {
        ApplicationItem,
        UrlItem
    };

    LauncherItem(const QString &fileName);
    LauncherItem(const QUrl &url);
    ~LauncherItem();

    Type type() const;

    QString label() const;
    QString iconName() const;
    QUrl url() const;

    bool isActive() const;
    bool isRunning() const;
    bool isUrgent() const;
    bool isDraggable() const;
    bool isEditable() const;
    bool isRemovable() const;

    bool isCounterVisible() const;
    int counter() const;

public Q_SLOTS:
    void activate();
    void launchNewInstance();

Q_SIGNALS:
    void labelChanged(const QString &label);
    void iconNameChanged(const QString &iconName);
    void urlChanged(const QUrl &url);
    void activeChanged(bool active);
    void runningChanged(bool running);
    void urgentChanged(bool urgent);
    void draggableChanged(bool draggable);
    void editableChanged(bool editable);
    void removableChanged(bool removable);
    void counterVisibleChanged(bool value);
    void counterChanged(int value);

private:
    Q_DECLARE_PRIVATE(LauncherItem)
    LauncherItemPrivate *const d_ptr;
};

#endif // LAUNCHERITEM_H
