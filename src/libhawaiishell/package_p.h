/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2011 Aaron Seigo <aseigo@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef PACKAGE_P_H
#define PACKAGE_P_H

#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QWeakPointer>

#include "packagestructure.h"

namespace Hawaii {

namespace Shell {

class ContentStructure
{
public:
    ContentStructure()
        : directory(false),
          required(false)
    {
    }

    ContentStructure(const ContentStructure &other)
    {
        paths = other.paths;
        name = other.name;
        mimeTypes = other.mimeTypes;
        directory = other.directory;
        required = other.required;
    }

    QString found;
    QStringList paths;
    QString name;
    QStringList mimeTypes;
    bool directory : 1;
    bool required : 1;
};

class PackagePrivate : public QSharedData
{
public:
    PackagePrivate();
    PackagePrivate(const PackagePrivate &other);
    ~PackagePrivate();

    PackagePrivate &operator=(const PackagePrivate &rhs);

    void createPackageMetadata(const QString &path);
    QString unpack(const QString &filePath);
    void updateHash(const QString &basePath, const QString &subPath, const QDir &dir, QCryptographicHash &hash);

    QWeakPointer<PackageStructure> structure;
    QString path;
    QString tempRoot;
    QStringList contentsPrefixPaths;
    QString defaultPackageRoot;
    QHash<QString, QString> discoveries;
    QHash<QByteArray, ContentStructure> contents;
    QStringList mimeTypes;
    PluginMetadata *metadata;
    bool externalPaths : 1;
    bool valid : 1;
    bool checkedValid : 1;
};

} // namespace Shell

} // namespace Hawaii

#endif // PACKAGE_P_H
