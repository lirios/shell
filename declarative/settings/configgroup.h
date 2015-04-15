/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2011-2012 by Sebastian Kügler <sebas@kde.org>
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef CONFIGGROUP_H
#define CONFIGGROUP_H

#include <QObject>
#include <QQuickItem>
#include <QVariant>
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CONFIGGROUP)

class KConfigGroup;

class ConfigGroupPrivate;

class ConfigGroup : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(QStringList keyList READ keyList NOTIFY keyListChanged)
    Q_PROPERTY(QStringList groupList READ groupList NOTIFY groupListChanged)

public:
    ConfigGroup(QQuickItem* parent=0);
    ~ConfigGroup();

    KConfigGroup* configGroup();

    QString file() const;
    void setFile(const QString &filename);
    QString group() const;
    void setGroup(const QString &groupname);
    QStringList keyList() const;
    QStringList groupList() const;

    Q_INVOKABLE QVariant readEntry(const QString &key, const QVariant &defaultValue = QVariant(""));
    Q_INVOKABLE bool writeEntry(const QString &key, const QVariant &value);
    Q_INVOKABLE void deleteEntry(const QString& key);

Q_SIGNALS:
    void fileChanged();
    void groupChanged();
    void keyListChanged();
    void groupListChanged();
    void configChanged();

private:
    ConfigGroupPrivate* d;

    bool readConfigFile();

private Q_SLOTS:
    void sync();
    void syncFile(const QString &fileName);
};

#endif // CONFIGGROUP_H
