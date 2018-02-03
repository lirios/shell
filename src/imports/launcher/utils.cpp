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

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "utils.h"

/*!
 * Returns a list of XDG compliant paths for applications.
 * See https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 */
QStringList xdgApplicationsPaths()
{
    QStringList paths;
    QStringList xdgPaths;

    // Add XDG_DATA_HOME
    QString xdgDataHome = QFile::decodeName(qgetenv("XDG_DATA_HOME"));
    if (xdgDataHome.isEmpty())
        xdgDataHome = QDir::home().absoluteFilePath(QLatin1String(".local/share"));
    xdgPaths.append(xdgDataHome);

    // Add XDG_DATA_DIRS
    QString xdgDataDirs = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));
    if (xdgDataDirs.isEmpty())
        xdgDataDirs = QLatin1String("/usr/local/share/:/usr/share/");

    // Split ':' path separator from XDG data directories
    xdgPaths += xdgDataDirs.split(QLatin1Char(':'), QString::SkipEmptyParts);

    // Append the applications directory to all XDG data directories
    Q_FOREACH (const QString &xdgPath, xdgPaths) {
        QDir iconDir = QDir(xdgPath).filePath(QLatin1String("applications"));
        paths.append(iconDir.absolutePath());
    }

    // Remove duplicates
    paths.removeDuplicates();

    return paths;
}
