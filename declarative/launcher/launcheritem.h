/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtCore/QSet>
#include <QtQml/QQmlListProperty>

class ApplicationAction;
class ApplicationInfo;
class LauncherModel;

class LauncherItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ appId CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(bool pinned READ isPinned NOTIFY pinnedChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
public:
    LauncherItem(const QString &appId, QObject *parent = 0);
    explicit LauncherItem(const QString &appId, bool pinned, QObject *parent = 0);

    QString appId() const;
    QString desktopFileName() const;

    QString name() const;
    QString comment() const;
    QString iconName() const;

    bool isPinned() const;

    bool isRunning() const;

    bool isActive() const;
    void setActive(bool value);

    int count() const;
    int progress() const;

    Q_INVOKABLE bool launch();
    Q_INVOKABLE bool quit();

Q_SIGNALS:
    void pinnedChanged();
    void runningChanged();
    void activeChanged();
    void countChanged();
    void progressChanged();
    void launched();

private:
    QSet<pid_t> m_pids;
    bool m_pinned;
    bool m_running;
    bool m_active;
    int m_count;
    int m_progress;
    ApplicationInfo *m_info;

    void setPinned(bool value);
    void setRunning(bool value);

    friend class LauncherModel;
};

#endif // LAUNCHERITEM_H
