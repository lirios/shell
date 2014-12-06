/***************************************************************************
 *   Copyright (C) 2014 by Eike Hein <hein@kde.org>                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "appchooserplugin.h"
#include "abstractmodel.h"
#include "favoritesmodel.h"
#include "funnelmodel.h"
#include "processrunner.h"
#include "rootmodel.h"
#include "runnermodel.h"
#include "systemmodel.h"

#include <QtQml>

void KickerPlugin::registerTypes(const char *uri)
{
    // @uri org.hawaii.appchooser
    Q_ASSERT(uri == QLatin1String("org.hawaii.appchooser"));

    qmlRegisterType<AbstractModel>();

    qmlRegisterType<AppsModel>(uri, 0, 1, "AppsModel");
    qmlRegisterType<FavoritesModel>(uri, 0, 1, "FavoritesModel");
    qmlRegisterType<FunnelModel>(uri, 0, 1, "FunnelModel");
    qmlRegisterType<ProcessRunner>(uri, 0, 1, "ProcessRunner");
    qmlRegisterType<RootModel>(uri, 0, 1, "RootModel");
    qmlRegisterType<RunnerModel>(uri, 0, 1, "RunnerModel");
    qmlRegisterType<SystemModel>(uri, 0, 1, "SystemModel");
}
