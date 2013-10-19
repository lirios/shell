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
#include "dialogwindow.h"
#include "element.h"
#include "enums.h"
#include "registration.h"
#include "keybinding.h"
#include "launcheritem.h"
#include "launchermodel.h"
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

void Registration::registerQmlTypes()
{
    // @uri Hawaii.Shell
    const char *uri = "Hawaii.Shell";
    const int major = 0;
    const int minor = 2;

    // Window types
    qmlRegisterType<DialogWindow>(uri, major, minor, "DialogWindow");
    qmlRegisterType<ShellWindow>(uri, major, minor, "ShellWindow");
    qmlRegisterType<PopupWindow>(uri, major, minor, "PopupWindow");
    qmlRegisterType<TooltipWindow>(uri, major, minor, "TooltipWindow");

    // Launcher stuff
    qmlRegisterType<AppCategories>(uri, major, minor, "XdgCategoriesModel");
    qmlRegisterType<ApplicationsModel>(uri, major, minor, "ApplicationsModel");
    qmlRegisterType<LauncherModel>(uri, major, minor, "LauncherModel");
    qmlRegisterUncreatableType<LauncherItem>(uri, major, minor, "LauncherItem",
                                             QStringLiteral("Do not create LauncherItem"));
    qmlRegisterUncreatableType<AppInfo>(uri, major, minor, "AppInfo",
                                        QStringLiteral("Do not create AppInfo"));

    // Shell types
    qmlRegisterType<Shortcut>(uri, major, minor, "Shortcut");
    qmlRegisterUncreatableType<Window>(uri, major, minor, "Window",
                                       QStringLiteral("Cannot create Window"));
    qmlRegisterUncreatableType<Workspace>(uri, major, minor, "Workspace",
                                          QStringLiteral("Workspace"));
    qmlRegisterType<Element>(uri, major, minor, "Element");

    // Enums
    qmlRegisterUncreatableType<UserStatus>(uri, major, minor, "UserStatus",
                                           QStringLiteral("Do not create UserStatus"));
}

void Registration::registerFactories()
{
    // @uri Hawaii.Shell
    const char *uri = "Hawaii.Shell";
    const int major = 0;
    const int minor = 2;

    // Register service factories
    qmlRegisterUncreatableType<PowerManager>(uri, major, minor,
                                             "PowerManager",
                                             QStringLiteral("Do not create PowerManager"));
    ServiceFactory::registerFactory<PowerManager>();
    ServiceFactory::registerFactory<SessionManager>();
    ServiceFactory::registerFactory<VolumeControl>();
}
