/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#ifndef APPSMODEL_H
#define APPSMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QLoggingCategory>
#include <QtQml/QQmlComponent>

class QDomElement;
class AppEntry;

Q_DECLARE_LOGGING_CATEGORY(APPSMODEL)

class AppsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(NameFormat appNameFormat READ appNameFormat WRITE setAppNameFormat NOTIFY
                       appNameFormatChanged)
    Q_PROPERTY(QString categoryFilter READ categoryFilter WRITE setCategoryFilter NOTIFY
                       categoryFilterChanged)
    Q_ENUMS(Roles NameFormat)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        CommentRole,
        IconNameRole,
        DesktopFileRole,
        FilterInfoRole
    };

    enum NameFormat
    {
        NameOnly = 0,
        GenericNameOnly,
        NameAndGenericName,
        GenericNameAndName
    };

    AppsModel(QObject *parent = 0);
    ~AppsModel();

    NameFormat appNameFormat() const;
    void setAppNameFormat(NameFormat format);

    QString categoryFilter() const;
    void setCategoryFilter(const QString &filter);

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE bool trigger(const QString &desktopFile);

Q_SIGNALS:
    void refreshing();
    void refreshed();
    void appNameFormatChanged();
    void categoryFilterChanged();
    void appLaunched(const QString &desktopFile);

private:
    QList<AppEntry *> m_list;
    NameFormat m_nameFormat;
    QString m_categoryFilter;

    void refresh();
    void readMenu(const QDomElement &xml);
    void readAppLink(const QDomElement &xml);
};

QML_DECLARE_TYPE(AppsModel)

#endif // APPSMODEL_H
