/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml>

#include "multimediakeysclient.h"
#include "osdclient.h"

class ShellPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.Shell
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell"));

        const int versionMajor = 1;
        const int versionMinor = 0;

        qmlRegisterSingletonType<MultimediaKeysClient>(uri, versionMajor, versionMinor, "MultimediaKeysClient",
                                                       [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new MultimediaKeysClient();
        });
        qmlRegisterSingletonType<OsdClient>(uri, versionMajor, versionMinor, "OsdClient",
                                            [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new OsdClient();
        });
    }
};

#include "plugin.moc"
