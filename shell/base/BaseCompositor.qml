/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

import QtQuick 2.5
import GreenIsland 1.0 as GreenIsland

GreenIsland.WaylandCompositor {
    id: compositor

    property Component screenViewComponent
    property Component outputComponent: BaseOutput {
        screenViewComponent: compositor.screenViewComponent
    }

    readonly property alias outputs: __private.outputs
    readonly property alias primaryScreen: screenManager.primaryScreen

    property alias __private: __private

    QtObject {
        id: __private

        property variant outputs: []
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Alt+Backspace"
        onActivated: Qt.quit()
    }

    GreenIsland.ScreenManager {
        id: screenManager

        onScreenAdded: {
            var view = outputComponent.createObject(compositor, {
                "compositor": compositor,
                "nativeScreen": screen
            })

            __private.outputs.push(view)
        }
        onScreenRemoved: {
            var index = screenManager.indexOf(screen)

            if (index < __private.outputs.length) {
                var output = __private.outputs[index]
                __private.outputs.splice(index, 1)
                output.destroy()
            }
        }
        onPrimaryScreenChanged: {
            var index = screenManager.indexOf(screen)

            if (index < __private.outputs.length)
                compositor.defaultOutput = __private.outputs[index]
        }
    }
}
