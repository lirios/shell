/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>

#include "appinfo.h"

namespace Hawaii {

namespace Shell {

AppInfo::AppInfo(QObject *parent)
    : QObject(parent)
    , XdgDesktopFile()
{
}

QString AppInfo::genericName() const
{
    return localizedValue(QStringLiteral("GenericName")).toString();
}

QStringList AppInfo::categories() const
{
    return value(QStringLiteral("Categories")).toString().split(';', QString::SkipEmptyParts);
}

bool AppInfo::isExecutable() const
{
    QString tryExec = value(QStringLiteral("TryExec")).toString();
    QString execCommand = value(QStringLiteral("Exec")).toString();

    // If TryExec is empty fallback to the first Exec argument
    if (tryExec.isEmpty()) {
        QStringList args = execCommand.split(QLatin1Char(' '), QString::SkipEmptyParts);
        if (args.size() == 0)
            return false;
        tryExec = args.at(0);
    }

    // Check if a full path was provided
    QFileInfo info(tryExec);
    if (info.exists())
        return QFileInfo(tryExec).isExecutable();

    // Find the executable from the system paths
    info.setFile(QStandardPaths::findExecutable(tryExec));
    if (info.exists())
        return info.isExecutable();

    return false;
}

bool AppInfo::isHidden() const
{
    // Application exists but should be hidden in the menu
    if (value(QStringLiteral("NoDisplay")).toBool())
        return true;

    // User deleted this application at his level
    if (value(QStringLiteral("Hidden")).toBool())
        return true;

    return false;
}

void AppInfo::launch()
{
    startDetached();
}

} // namespace Shell

} // namespace Hawaii

#include "moc_appinfo.cpp"
