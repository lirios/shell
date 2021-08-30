// SPDX-FileCopyrightText: 2019-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FREQUENTMODEL_H
#define FREQUENTMODEL_H

#include <QSortFilterProxyModel>
#include <QQmlComponent>

class FrequentAppsModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FrequentAppsModel(QObject *parent = nullptr);

    int startIndex() const;
    int limitCount() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

QML_DECLARE_TYPE(FrequentAppsModel)

#endif // FREQUENTMODEL_H
