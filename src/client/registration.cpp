/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include <QtQml/QQmlComponent>

#include <HawaiiShell/HawaiiShell>

#include "enums.h"
#include "registration.h"
#include "keybinding.h"
#include "shellcontroller.h"
#include "powermanager.h"
#include "processlauncher.h"
#include "servicefactory.h"
#include "sessionmanager.h"
#include "shortcut.h"
#include "window.h"
#include "workspace.h"
#include "windowtypes/overlaywindow.h"

static QObject *sessionManagerProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new SessionManager();
}

static QObject *powerManagerProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new PowerManager();
}

static QObject *processLauncherProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new ProcessLauncher();
}

void Registration::registerQmlTypes()
{
    // @uri Hawaii.Shell
    const char *uri = "Hawaii.Shell";

    // Singletons
    qmlRegisterSingletonType<SessionManager>(uri, 1, 0, "SessionManager", sessionManagerProvider);
    qmlRegisterSingletonType<PowerManager>(uri, 1, 0, "PowerManager", powerManagerProvider);
    qmlRegisterSingletonType<ProcessLauncher>(uri, 1, 0, "ProcessLauncher", processLauncherProvider);

    // Shell types
    qmlRegisterType<Shortcut>(uri, 1, 0, "Shortcut");
    qmlRegisterUncreatableType<Window>(uri, 1, 0, "Window",
                                       QStringLiteral("Cannot create Window"));
    qmlRegisterUncreatableType<Workspace>(uri, 1, 0, "Workspace",
                                          QStringLiteral("Workspace"));

    // Enums
    qmlRegisterUncreatableType<UserStatus>(uri, 1, 0, "UserStatus",
                                           QStringLiteral("Do not create UserStatus"));
    qmlRegisterUncreatableType<Hawaii::Types>(uri, 1, 0, "Types",
                                              QStringLiteral("Do not create Types"));
}

void Registration::registerPrivateQmlTypes()
{
    // @uri Hawaii.Shell.Private
    const char *uri = "Hawaii.Shell.Private";

    qmlRegisterType<OverlayWindow>(uri, 1, 0, "OverlayWindow");
}
