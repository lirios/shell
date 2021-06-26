// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef STORAGEDEVICE_H
#define STORAGEDEVICE_H

#include <QObject>

#include <Solid/Device>

class StorageDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString udi READ udi CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(QString filePath READ filePath CONSTANT)
    Q_PROPERTY(bool mounted READ isMounted NOTIFY mountedChanged)
    Q_PROPERTY(bool ignored READ isIgnored CONSTANT)
public:
    explicit StorageDevice(const QString &udi, QObject *parent = nullptr);

    QString udi() const;
    QString name() const;
    QString iconName() const;
    QString filePath() const;

    bool isMounted() const;
    Q_INVOKABLE void mount();
    Q_INVOKABLE void unmount();

    bool isIgnored() const;

Q_SIGNALS:
    void mountedChanged();

private:
    Solid::Device m_device;
    QString m_filePath;
    bool m_ignored;
};

#endif // STORAGEDEVICE_H
