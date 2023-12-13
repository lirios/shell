// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDir>
#include <QFile>

#include "xdgutils.h"

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
    xdgPaths += xdgDataDirs.split(QLatin1Char(':'), Qt::SkipEmptyParts);

    // Append the applications directory to all XDG data directories
    for (const QString &xdgPath : qAsConst(xdgPaths)) {
        QDir iconDir = QDir(xdgPath).filePath(QLatin1String("applications"));
        paths.append(iconDir.absolutePath());
    }

    // Remove duplicates
    paths.removeDuplicates();

    return paths;
}
