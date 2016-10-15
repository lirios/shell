/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

class Application;
class ApplicationManager;

class LauncherModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ApplicationManager *applicationManager READ applicationManager WRITE setApplicationManager NOTIFY applicationManagerChanged)
public:
    enum Roles
    {
        AppIdRole = Qt::UserRole + 1,
        DesktopFileRole,
        NameRole,
        CommentRole,
        IconNameRole,
        PinnedRole,
        RunningRole,
        StartingRole,
        ActiveRole,
        HasWindowsRole,
        HasCountRole,
        CountRole,
        HasProgressRole,
        ProgressRole
    };
    Q_ENUM(Roles)

    LauncherModel(QObject *parent = nullptr);

    ApplicationManager *applicationManager() const;

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE Application *get(int index) const;
    Q_INVOKABLE int indexFromAppId(const QString &appId) const;

public Q_SLOTS:
    void setApplicationManager(ApplicationManager *appMan);
    void setupConnections(Application *app);

Q_SIGNALS:
    void applicationManagerChanged();

private:
    ApplicationManager *m_appMan = nullptr;
    QList<Application *> m_apps;

    int pinAtIndex() const;

    bool moveRows(int sourceRow, int count, int destinationChild);
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE;
};

QML_DECLARE_TYPE(LauncherModel)

#endif // LAUNCHERMODEL_H
