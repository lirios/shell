// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef STORAGEMODEL_H
#define STORAGEMODEL_H

#include <QAbstractListModel>

class StorageDevice;

class StorageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles {
        StorageDeviceRole = Qt::UserRole + 1,
        UdiRole,
        NameRole,
        IconNameRole,
        FilePathRole,
        IsMountedRole,
        IsIgnoredRole
    };

    explicit StorageModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void countChanged();

private:
    QVector<StorageDevice *> m_devices;
    QMap<QString, StorageDevice *> m_devicesMap;
};

#endif // STORAGEMODEL_H
