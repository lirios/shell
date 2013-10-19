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

#include "appcategories.h"
#include "appinfo.h"
#include "applicationsmodel.h"
#include "backgroundsettings.h"
#include "dialogwindow.h"
#include "element.h"
#include "enums.h"
#include "registration.h"
#include "keybinding.h"
#include "launcheritem.h"
#include "launchermodel.h"
#include "launchersettings.h"
#include "popupwindow.h"
#include "powermanager.h"
#include "servicefactory.h"
#include "sessionmanager.h"
#include "shellwindow.h"
#include "shortcut.h"
#include "tooltipwindow.h"
#include "volumecontrol.h"
#include "window.h"
#include "workspace.h"

#define PLUGIN_URI "Hawaii.Shell"
#define PLUGIN_VERSION 0, 2

void registerQmlTypes()
{
    // Window types
    qmlRegisterType<DialogWindow>(PLUGIN_URI, PLUGIN_VERSION, "DialogWindow");
    qmlRegisterType<ShellWindow>(PLUGIN_URI, PLUGIN_VERSION, "ShellWindow");
    qmlRegisterType<PopupWindow>(PLUGIN_URI, PLUGIN_VERSION, "PopupWindow");
    qmlRegisterType<TooltipWindow>(PLUGIN_URI, PLUGIN_VERSION, "TooltipWindow");

    // Settings
    qmlRegisterType<BackgroundSettings>(PLUGIN_URI, PLUGIN_VERSION, "BackgroundSettings");
    qmlRegisterType<LauncherSettings>(PLUGIN_URI, PLUGIN_VERSION, "LauncherSettings");

    // Launcher stuff
    qmlRegisterType<AppCategories>(PLUGIN_URI, PLUGIN_VERSION, "XdgCategoriesModel");
    qmlRegisterType<ApplicationsModel>(PLUGIN_URI, PLUGIN_VERSION, "ApplicationsModel");
    qmlRegisterType<LauncherModel>(PLUGIN_URI, PLUGIN_VERSION, "LauncherModel");
    qmlRegisterUncreatableType<LauncherItem>(PLUGIN_URI, PLUGIN_VERSION, "LauncherItem",
                                             QStringLiteral("Cannot create LauncherItem"));
    qmlRegisterUncreatableType<AppInfo>(PLUGIN_URI, PLUGIN_VERSION, "AppInfo",
                                        QStringLiteral("Cannot create AppInfo"));

    // Shell types
    qmlRegisterType<Shortcut>(PLUGIN_URI, PLUGIN_VERSION, "Shortcut");
    qmlRegisterUncreatableType<Window>(PLUGIN_URI, PLUGIN_VERSION, "Window",
                                       QStringLiteral("Cannot create Window"));
    qmlRegisterUncreatableType<Workspace>(PLUGIN_URI, PLUGIN_VERSION, "Workspace",
                                          QStringLiteral("Workspace"));
    qmlRegisterType<Element>(PLUGIN_URI, PLUGIN_VERSION, "Element");

    // Enums
    qmlRegisterUncreatableType<UserStatus>(PLUGIN_URI, PLUGIN_VERSION, "UserStatus",
                                           QStringLiteral("Cannot create UserStatus"));
}

void registerFactories()
{
    // Register service factories
    qmlRegisterUncreatableType<PowerManager>(PLUGIN_URI, PLUGIN_VERSION,
                                             "PowerManager",
                                             QStringLiteral("Cannot create PowerManager"));
    ServiceFactory::registerFactory<PowerManager>();
    ServiceFactory::registerFactory<SessionManager>();
    ServiceFactory::registerFactory<VolumeControl>();
}
