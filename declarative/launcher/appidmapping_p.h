/****************************************************************************
* This file is part of Hawaii.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini
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

#ifndef APPIDMAPPING_P_H
#define APPIDMAPPING_P_H

#include <QtCore/QMap>
#include <QtCore/QStandardPaths>

class AppIdMapping
{
public:
    static QString mapAppId(const QString &origAppId)
    {
        // Map known identifiers to the correct ones
        QMap<QString, QString> map;
        map[QLatin1String("org.hawaiios.hawaii-system-preferences")] = QLatin1String("org.hawaiios.SystemPreferences");
        if (map.contains(origAppId))
            return map[origAppId];
        return origAppId;
    }

    static QString appIdToName(const QString &appId)
    {
        // Replace '-' with '/' as mentioned here:
        // http://standards.freedesktop.org/desktop-entry-spec/1.1/ape.html
        // except for special cases
        QStringList dashExclusions;
        dashExclusions.append(QLatin1String("qterminal-qt5"));
        return dashExclusions.contains(appId) ? appId : QString(appId).replace('-', '/');
    }

    static QString desktopFileName(const QString &appId)
    {
        const QString name = appIdToName(appId);
        QString fileName;

        // Now the fun part begins: Qt sets app_id to program name + ".desktop",
        // while GTK+ uses the program name starting with an upper case letter.
        // It seems a pattern with D-Bus activatable it goes like this: let's take
        // GNOME Clocks as an example: Gnome-clocks app_id and
        // org.gnome.clocks.desktop desktop entry.

        // Case 1: assume app_id is already the desktop file base name
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          name);
        if (!fileName.isEmpty())
            return fileName;

        // Case 2: try without changing the case appending ".desktop"
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          name + QStringLiteral(".desktop"));
        if (!fileName.isEmpty())
            return fileName;

        // Case 3: lower case and append ".desktop"
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          name.toLower() + QStringLiteral(".desktop"));
        if (!fileName.isEmpty())
            return fileName;

        // Case 4: lower case, do not replace '-' with '/' and append ".desktop" (GTK+)
        fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                          appId.toLower() + QStringLiteral(".desktop"));
        if (!fileName.isEmpty())
            return fileName;

        // Case 5: D-Bus activatable applications: lower case appId, split with '-'
        // and treat the first as a reverse domain name prepending org (yeah it might
        // be another extension...)
        QStringList pieces = appId.toLower().split('-', QString::SkipEmptyParts);
        if (pieces.size() == 2) {
            QString processedAppId = QStringLiteral("org.%1.%2")
                    .arg(pieces.at(0)).arg(pieces.at(1));
            fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                              processedAppId + QStringLiteral(".desktop"));
        }

        return fileName;
    }
};

#endif // APPIDMAPPING_P_H
