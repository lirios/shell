/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQml 2.2
import QtQuick 2.5
import Liri.WaylandServer 1.0 as LiriWayland

Instantiator {
    id: screenManager

    property alias screenModel: screenModel

    model: LiriWayland.ScreenModel {
        id: screenModel
        fileName: screenConfigurationFileName
    }
    onObjectRemoved: {
        // Move all windows that fit entirely the removed output to the primary output,
        // unless the output remove is the primary one (this shouldn't happen)
        if (object === liriCompositor.defaultOutput)
            return;
        for (var surface in object.viewsBySurface) {
            var view = object.viewsBySurface[surface];
            if (view.primary && view.output === object) {
                view.moveItem.x = liriCompositor.defaultOutput.position.x + 20;
                view.moveItem.y = liriCompositor.defaultOutput.position.y + 20;
            }
        }
    }
}
