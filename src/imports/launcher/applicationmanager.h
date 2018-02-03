/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
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

#pragma once

#include <QtCore/QLoggingCategory>
#include <QtCore/QAbstractListModel>

#include <Qt5GSettings/QGSettings>

class QDomElement;
class Application;

using namespace QtGSettings;

Q_DECLARE_LOGGING_CATEGORY(APPLICATION_MANAGER)

class ApplicationManager : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        AppIdRole = Qt::UserRole + 1,
        ApplicationRole,
        DesktopFileRole,
        NameRole,
        GenericNameRole,
        CommentRole,
        IconNameRole,
        CategoriesRole,
        FilterInfoRole,
        PinnedRole,
        PinnedIndexRole,
        RunningRole,
        StartingRole,
        ActiveRole,
        HasWindowsRole,
        HasCountRole,
        CountRole,
        HasProgressRole,
        ProgressRole,
        ActionsRole
    };
    Q_ENUM(Roles)

    explicit ApplicationManager(QObject *parent = nullptr);
    virtual ~ApplicationManager();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Application *findApplication(const QString &appId);
    Application *newApplication(const QString &appId);

    Q_INVOKABLE void registerShellSurface(QObject *shellSurface);
    Q_INVOKABLE void unregisterShellSurface(QObject *shellSurface);
    Q_INVOKABLE void focusShellSurface(QObject *shellSurface);

    Q_INVOKABLE QString canonicalizeAppId(const QString &appId);

    Q_INVOKABLE Application *get(int index) const;

    Q_INVOKABLE QString getIconName(const QString &appId);
    Q_INVOKABLE int indexFromAppId(const QString &appId) const;

    static void refresh(ApplicationManager *manager);

public Q_SLOTS:
    void launch(const QString &appId);
    void quit(const QString &appId);

Q_SIGNALS:
    void refreshed();
    void applicationAdded(Application *app);
    void applicationRemoved(Application *app);
    void applicationPinned(Application *app);
    void applicationUnpinned(Application *app);
    void applicationLaunched(Application *app);

private:
    QGSettings *m_settings = nullptr;
    QList<Application *> m_apps;
    QMap<QObject *, QString> m_shellSurfaces;

private Q_SLOTS:
    void addApp(const QString &appId, const QString &categoryName);
    void removeApp(QObject *object);
};
