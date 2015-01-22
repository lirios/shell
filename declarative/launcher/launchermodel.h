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

#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

#include <GreenIsland/ApplicationManager>

class LauncherItem;

class LauncherModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(GreenIsland::ApplicationManager *applicationManager READ applicationManager WRITE setApplicationManager NOTIFY applicationManagerChanged)
    Q_ENUMS(Roles)
public:
    enum Roles {
        AppIdRole = Qt::UserRole + 1,
        NameRole,
        CommentRole,
        IconNameRole,
        PinnedRole,
        RunningRole,
        ActiveRole,
        HasWindowsRole,
        HasCountRole,
        CountRole,
        HasProgressRole,
        ProgressRole,
        HasActionsRole
    };

    LauncherModel(QObject *parent = 0);
    ~LauncherModel();

    GreenIsland::ApplicationManager *applicationManager() const;
    void setApplicationManager(GreenIsland::ApplicationManager *appMan);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE LauncherItem *get(int index) const;

Q_SIGNALS:
    void applicationManagerChanged();

private:
    QList<LauncherItem *> m_list;
    GreenIsland::ApplicationManager *m_appMan;
};

QML_DECLARE_TYPE(LauncherModel)

#endif // LAUNCHERMODEL_H
