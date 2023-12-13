// SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2018 Pierre-Yves Siret
// SPDX-License-Identifier: MIT

#pragma once

#include <QQmlExpression>
#include <QRegularExpression>
#include <QSortFilterProxyModel>

class QQmlSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString filterRoleName READ filterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)
    Q_PROPERTY(
        QString filterPattern READ filterPattern WRITE setFilterPattern NOTIFY filterPatternChanged)
    Q_PROPERTY(QVariant filterValue READ filterValue WRITE setFilterValue NOTIFY filterValueChanged)
    Q_PROPERTY(QQmlScriptString filterExpression READ filterExpression WRITE setFilterExpression
                   NOTIFY filterExpressionChanged)
    Q_PROPERTY(
        QString sortRoleName READ sortRoleName WRITE setSortRoleName NOTIFY sortRoleNameChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(QQmlScriptString sortExpression READ sortExpression WRITE setSortExpression NOTIFY sortExpressionChanged)
    QML_NAMED_ELEMENT(SortFilterProxyModel)
public:
    QQmlSortFilterProxyModel(QObject *parent = nullptr);

    int count() const;

    QHash<int, QByteArray> roleNames() const override;

    const QString &filterRoleName() const;
    void setFilterRoleName(const QString &filterRoleName);

    QString filterPattern() const;
    void setFilterPattern(const QString &filterPattern);

    const QVariant &filterValue() const;
    void setFilterValue(const QVariant &filterValue);

    const QQmlScriptString &filterExpression() const;
    void setFilterExpression(const QQmlScriptString &filterScriptString);

    const QString &sortRoleName() const;
    void setSortRoleName(const QString &sortRoleName);

    void setSortOrder(Qt::SortOrder sortOrder);

    const QQmlScriptString &sortExpression() const;
    void setSortExpression(const QQmlScriptString &compareScriptString);

signals:
    void countChanged();

    void filterRoleNameChanged();
    void filterPatternChanged();
    void filterValueChanged();
    void filterExpressionChanged();

    void sortRoleNameChanged();
    void sortOrderChanged();
    void sortExpressionChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private slots:
    void invalidateFilter();
    void updateFilterRole();
    void updateSortRole();
    void updateRoles();

private:
    QVariantMap modelDataMap(const QModelIndex &modelIndex) const;

    QString m_filterRoleName;
    QString m_sortRoleName;

    QQmlScriptString m_filterScriptString;
    QQmlExpression *m_filterExpression;

    QQmlScriptString m_compareScriptString;
    QQmlExpression *m_compareExpression;
    QVariant m_filterValue;
};
