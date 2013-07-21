/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml/QQmlComponent>

#include "declarativeplugin.h"
#include "enums.h"
#include "appcategories.h"
#include "applicationsmodel.h"
#include "launchermodel.h"
#include "launcheritem.h"
#include "launchersettings.h"
#include "shellwindow.h"
#include "notificationwindow.h"

void registerQmlTypes()
{
    qmlRegisterType<AppCategories>("Hawaii.Shell.Desktop", 0, 1, "XdgCategoriesModel");
    qmlRegisterType<ApplicationsModelSorted>("Hawaii.Shell.Desktop", 0, 1, "ApplicationsModel");
    qmlRegisterType<LauncherModel>("Hawaii.Shell.Desktop", 0, 1, "LauncherModel");
    qmlRegisterUncreatableType<LauncherItem>("Hawaii.Shell.Desktop", 0, 1, "LauncherItem",
                                             QStringLiteral("Cannot create LauncherItem"));
    qmlRegisterType<LauncherSettings>("Hawaii.Shell.Desktop", 0, 1, "LauncherSettings");
    qmlRegisterUncreatableType<LauncherAlignment>("Hawaii.Shell.Desktop", 0, 1, "LauncherAlignment",
                                                  QStringLiteral("Cannot create LauncherAlignment"));
    qmlRegisterType<ShellWindow>("Hawaii.Shell.Desktop", 0, 1, "ShellWindow");
    qmlRegisterType<NotificationWindow>("Hawaii.Shell.Desktop", 0, 1, "NotificationWindow");
}
