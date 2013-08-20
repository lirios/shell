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

#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

class QUrl;

class LauncherSettings;
class LauncherItem;

class LauncherModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum {
        IsApplicationRole = Qt::UserRole + 1,
        IsUrlRole,
        LabelRole,
        IconNameRole,
        UrlRole,
        IsActiveRole,
        IsRunningRole,
        IsUrgenteRole,
        IsDraggableRole,
        IsEditableRole,
        IsRemovableRole,
        IsCounterVisibleRole,
        CounterRole
    };

    LauncherModel(QObject *parent = 0);
    ~LauncherModel();

    QHash<int, QByteArray> roleNames() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

public Q_SLOTS:
    LauncherItem *get(int index);

    void pinApplication(const QString &fileName);
    void pinUrl(const QUrl &url);

private:
    LauncherSettings *m_settings;
    QList<LauncherItem *> m_list;
};

QML_DECLARE_TYPE(LauncherModel)

#endif // LAUNCHERMODEL_H
