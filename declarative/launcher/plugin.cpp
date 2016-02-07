/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

#include <QtQml/QtQml>

#include "appsmodel.h"
#include "appsproxymodel.h"
#include "categoriesmodel.h"
#include "launchermodel.h"
#include "launcheritem.h"
#include "menuimageprovider.h"
#include "processrunner.h"

class LauncherPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("org.hawaiios.launcher"));

        engine->addImageProvider(QStringLiteral("launcher"), new MenuImageProvider());
    }

    void registerTypes(const char *uri)
    {
        // @uri org.hawaiios.launcher
        Q_ASSERT(uri == QLatin1String("org.hawaiios.launcher"));

        qmlRegisterType<AppsModel>(uri, 0, 1, "AppsModel");
        qmlRegisterType<AppsProxyModel>(uri, 0, 1, "AppsProxyModel");
        qmlRegisterType<CategoriesModel>(uri, 0, 1, "CategoriesModel");
        qmlRegisterType<LauncherModel>(uri, 0, 1, "LauncherModel");
        qmlRegisterUncreatableType<LauncherItem>(uri, 0, 1, "LauncherItem",
                                                 QStringLiteral("Cannot instantiate LauncherItem"));
        qmlRegisterType<ProcessRunner>(uri, 0, 1, "ProcessRunner");
    }
};

#include "plugin.moc"
