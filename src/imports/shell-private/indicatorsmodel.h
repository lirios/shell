/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef INDICATORSMODEL_H
#define INDICATORSMODEL_H

#include <QAbstractListModel>
#include <QUrl>

class IndicatorItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
public:
    explicit IndicatorItem(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    QUrl url() const;
    void setUrl(const QUrl &url);

private:
    QString m_name;
    QUrl m_url;
};

class IndicatorsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        UrlRole
    };

    explicit IndicatorsModel(QObject *parent = nullptr);
    ~IndicatorsModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    static bool itemLessThan(const IndicatorItem *i1, const IndicatorItem *i2);
    static void populate(IndicatorsModel *model);

Q_SIGNALS:
    void addItem(IndicatorItem *item);
    void ready();

private:
    QHash<int, QByteArray> m_roleNames;
    QList<IndicatorItem*> m_items;
    QStringList m_indicatorNames;

private Q_SLOTS:
    void addNewItem(QObject *object);
    void modelReady();
};

#endif // INDICATORSMODEL_H
