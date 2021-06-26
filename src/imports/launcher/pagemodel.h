// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtCore/QSortFilterProxyModel>
#include <QtQml/QQmlComponent>

/**
 * Provides a simple proxy model for accessing a subset, or "page," of data from a source model.
 * This is used by PagedGrid to provide models to each page with the appropriate subset of data.
 */
class PageModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex NOTIFY startIndexChanged)
    Q_PROPERTY(int limitCount READ limitCount WRITE setLimitCount NOTIFY limitCountChanged)

public:
    PageModel(QObject *parent = nullptr);

    int startIndex() const;
    int limitCount() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public Q_SLOTS:
    void setStartIndex(int startIndex);
    void setLimitCount(int limitCount);

Q_SIGNALS:
    void startIndexChanged();
    void limitCountChanged();

private:
    int m_startIndex = 0;
    int m_limitCount = 0;
};

QML_DECLARE_TYPE(PageModel)
