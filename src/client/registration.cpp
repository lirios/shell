/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <HawaiiShell/HawaiiShell>

#include "appcategories.h"
#include "appinfo.h"
#include "applicationsmodel.h"
#include "datetime.h"
#include "enums.h"
#include "registration.h"
#include "keybinding.h"
#include "shellcontroller.h"
#include "policykitagent.h"
#include "powermanager.h"
#include "processlauncher.h"
#include "servicefactory.h"
#include "sessionmanager.h"
#include "shortcut.h"
#include "volumecontrol.h"
#include "window.h"
#include "workspace.h"

void Registration::registerQmlTypes()
{
    // @uri Hawaii.Shell
    const char *uri = "Hawaii.Shell";

    // Launcher stuff
    qmlRegisterType<AppCategories>(uri, 1, 0, "XdgCategoriesModel");
    qmlRegisterType<ApplicationsModel>(uri, 1, 0, "ApplicationsModel");
    qmlRegisterUncreatableType<AppInfo>(uri, 1, 0, "AppInfo",
                                        QStringLiteral("Do not create AppInfo"));

    // Shell types
    qmlRegisterType<Shortcut>(uri, 1, 0, "Shortcut");
    qmlRegisterUncreatableType<Window>(uri, 1, 0, "Window",
                                       QStringLiteral("Cannot create Window"));
    qmlRegisterUncreatableType<Workspace>(uri, 1, 0, "Workspace",
                                          QStringLiteral("Workspace"));

    // Enums
    qmlRegisterUncreatableType<UserStatus>(uri, 1, 0, "UserStatus",
                                           QStringLiteral("Do not create UserStatus"));
    qmlRegisterUncreatableType<Hawaii::Shell::Types>(uri, 1, 0, "Types",
                                                     QStringLiteral("Do not create Types"));
}

void Registration::registerFactories()
{
    // @uri Hawaii.Shell
    const char *uri = "Hawaii.Shell";

    // Register some services as types for using on QML
    qmlRegisterUncreatableType<PowerManager>(uri, 1, 0,
                                             "PowerManager",
                                             QStringLiteral("Do not create PowerManager"));
    qmlRegisterUncreatableType<PolicyKitAgent>(uri, 1, 0,
                                               "PolicyKitAgent",
                                               QStringLiteral("Do not create PolicyKitAgent"));

    // Register service factories
    ServiceFactory::registerFactory<DateTime>();
    ServiceFactory::registerFactory<PowerManager>();
    ServiceFactory::registerFactory<ProcessLauncher>();
    ServiceFactory::registerFactory<SessionManager>();
    ServiceFactory::registerFactory<VolumeControl>();
    ServiceFactory::registerFactory<PolicyKitAgent>();
}
