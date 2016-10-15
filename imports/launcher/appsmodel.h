/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

#include "applicationmanager.h"

class AppsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ApplicationManager *applicationManager READ applicationManager WRITE setApplicationManager NOTIFY applicationManagerChanged)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        GenericNameRole,
        CommentRole,
        IconNameRole,
        AppIdRole,
        CategoriesRole,
        RunningRole,
        FilterInfoRole
    };
    Q_ENUM(Roles)

    AppsModel(QObject *parent = nullptr);

    ApplicationManager *applicationManager() const;

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void setApplicationManager(ApplicationManager *appMan);

Q_SIGNALS:
    void appLaunched(Application *app);
    void applicationManagerChanged();

private Q_SLOTS:
    void setupConnections(Application *app);

private:
    ApplicationManager *m_appMan = nullptr;
    QList<Application *> m_apps;
};

QML_DECLARE_TYPE(AppsModel)
