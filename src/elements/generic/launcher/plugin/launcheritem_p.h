/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LAUNCHERITEM_P_H
#define LAUNCHERITEM_P_H

#include <HawaiiShell/AppInfo>

#include "launcheritem.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Hawaii Shell API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class LauncherItemPrivate
{
public:
    LauncherItemPrivate();
    ~LauncherItemPrivate();

    LauncherItem::Type type;
    Hawaii::Shell::AppInfo *appInfo;
    QUrl url;
    bool isActive;
    bool isRunning;
    bool isUrgent;
    bool isDraggable;
    bool isEditable;
    bool isRemovable;
    bool isCounterVisible;
    int counter;
};

#endif // LAUNCHERITEM_P_H
