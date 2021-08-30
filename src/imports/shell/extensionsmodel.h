// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EXTENSIONSMODEL_H
#define EXTENSIONSMODEL_H

#include <QAbstractListModel>
#include <QLoggingCategory>
#include <QUrl>

Q_DECLARE_LOGGING_CATEGORY(lcExtensions)

class ExtensionItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(quint32 order READ order CONSTANT)
public:
    explicit ExtensionItem(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    QUrl url() const;
    void setUrl(const QUrl &url);

    quint32 order() const;
    void setOrder(quint32 value);

private:
    QString m_name;
    QUrl m_url;
    quint32 m_order = 0;
};

class ExtensionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        UrlRole,
        OrderRole
    };

    explicit ExtensionsModel(QObject *parent = nullptr);
    ~ExtensionsModel();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    static void populate(ExtensionsModel *model);

private:
    QHash<int, QByteArray> m_roleNames;
    QList<ExtensionItem*> m_items;
    QStringList m_extensionNames;
};

#endif // EXTENSIONSMODEL_H
