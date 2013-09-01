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

#include <QtXdg/QApplicationInfo>

#include "appcategories.h"
#include "applicationsmodel.h"
#include "backgroundsettings.h"
#include "enums.h"
#include "registration.h"
#include "keybinding.h"
#include "launcheritem.h"
#include "launchermodel.h"
#include "launchersettings.h"
#include "powermanager.h"
#include "servicefactory.h"
#include "sessionmanager.h"
#include "shellwindow.h"
#include "shortcut.h"
#include "volumecontrol.h"
#include "window.h"
#include "workspace.h"

void registerQmlTypes()
{
    // @uri Hawaii.Shell.Desktop
    const char *uri = "Hawaii.Shell.Desktop";

    // Window types
    qmlRegisterType<ShellWindow>(uri, 0, 1, "ShellWindow");

    // Settings
    qmlRegisterType<BackgroundSettings>(uri, 0, 1, "BackgroundSettings");
    qmlRegisterType<LauncherSettings>(uri, 0, 1, "LauncherSettings");

    // Launcher stuff
    qmlRegisterType<AppCategories>(uri, 0, 1, "XdgCategoriesModel");
    qmlRegisterType<ApplicationsModel>(uri, 0, 1, "ApplicationsModel");
    qmlRegisterType<LauncherModel>(uri, 0, 1, "LauncherModel");
    qmlRegisterUncreatableType<LauncherItem>(uri, 0, 1, "LauncherItem",
                                             QStringLiteral("Cannot create LauncherItem"));
    qmlRegisterUncreatableType<QApplicationInfo>(uri, 0, 1, "ApplicationInfo",
                                             QStringLiteral("Cannot create ApplicationInfo"));

    // Shell types
    qmlRegisterType<KeyBinding>();
    qmlRegisterType<Shortcut>(uri, 0, 1, "Shortcut");
    qmlRegisterUncreatableType<Window>(uri, 0, 1, "Window",
                                       QStringLiteral("Cannot create Window"));
    qmlRegisterUncreatableType<Workspace>(uri, 0, 1, "Workspace",
                                          QStringLiteral("Workspace"));

    // Enums
    qmlRegisterUncreatableType<UserStatus>(uri, 0, 1, "UserStatus",
                                           QStringLiteral("Cannot create UserStatus"));
}

void registerFactories()
{
    // Register service factories
    qmlRegisterUncreatableType<PowerManager>("Hawaii.Shell.Desktop", 0, 1,
                                             "PowerManager",
                                             QStringLiteral("Cannot create PowerManager"));
    ServiceFactory::registerFactory<PowerManager>();
    ServiceFactory::registerFactory<SessionManager>();
    ServiceFactory::registerFactory<VolumeControl>();
}
