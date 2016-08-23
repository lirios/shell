/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml/QtQml>

#include "application.h"
#include "applicationmanager.h"
#include "appsmodel.h"
#include "categoriesmodel.h"
#include "frequentmodel.h"
#include "launchermodel.h"
#include "pagemodel.h"
#include "processrunner.h"

class LauncherPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri)
    {
        // @uri org.hawaiios.launcher
        Q_ASSERT(uri == QLatin1String("org.hawaiios.launcher"));

        qmlRegisterType<ApplicationManager>(uri, 0, 1, "ApplicationManager");
        qmlRegisterType<AppsModel>(uri, 0, 1, "AppsModel");
        qmlRegisterType<CategoriesModel>(uri, 0, 1, "CategoriesModel");
        qmlRegisterType<LauncherModel>(uri, 0, 1, "LauncherModel");
        qmlRegisterType<PageModel>(uri, 0, 1, "PageModel");
        qmlRegisterType<FrequentAppsModel>(uri, 0, 1, "FrequentAppsModel");
        qmlRegisterType<ProcessRunner>(uri, 0, 1, "ProcessRunner");
        qmlRegisterUncreatableType<Application>(uri, 0, 1, "Application",
                                                QStringLiteral("Cannot instantiate Application"));
    }
};

#include "plugin.moc"
