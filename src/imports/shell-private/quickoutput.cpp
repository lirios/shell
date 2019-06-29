/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "quickoutput.h"

QuickOutput::QuickOutput()
    : QWaylandQuickOutput()
{
}

void QuickOutput::addOutputMode(const QSize &size, int refresh, bool isPreferred, bool isCurrent)
{
    QWaylandOutputMode wlMode(size, refresh);

    bool alreadyHasMode = false;
    const auto modesList = modes();
    for (const auto &mode : modesList) {
        if (mode == wlMode) {
            alreadyHasMode = true;
            break;
        }
    }

    if (!alreadyHasMode)
        addMode(wlMode, isPreferred);

    if (isCurrent)
        setCurrentMode(wlMode);
}

void QuickOutput::setCurrentOutputMode(const QSize &size, int refresh)
{
    QWaylandOutputMode wlMode(size, refresh);
    setCurrentMode(wlMode);
}
