/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef PACKAGESMODELITEM_H
#define PACKAGESMODELITEM_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <HawaiiShell/Package>

class PackagesModelItemPrivate;

class PackagesModelItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString internalName READ internalName CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
public:
    explicit PackagesModelItem(Hawaii::Shell::Package package,
                               QObject *parent = 0);
    ~PackagesModelItem();

    QString internalName() const;
    QString name() const;
    QString comment() const;

    Q_INVOKABLE QUrl filePath(const QString &key,
                              const QString &fileName = QString()) const;

private:
    Q_DECLARE_PRIVATE(PackagesModelItem)
    PackagesModelItemPrivate *const d_ptr;
};

#endif // PACKAGESMODELITEM_H
