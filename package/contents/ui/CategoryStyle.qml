/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.0 as Styles
import QtQuick.Controls.Private 1.0
import org.kde.plasma.core 2.0 as PlasmaCore

Styles.ButtonStyle {
    padding {
        left: 0
        top: 0
        right: 0
        bottom: 0
    }

    background: Item {
        property bool checked: control.checkable && control.checked
        property bool down: control.pressed || checked

        implicitWidth: Math.round(TextSingleton.implicitHeight * 2.5)
        implicitHeight: Math.max(units.gridUnit * 2, Math.round(TextSingleton.implicitHeight * 1.2))

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: checked ? control.hovered ? Qt.lighter(__syspal.highlight, 1.5) : __syspal.highlight : Qt.lighter(__syspal.highlight, 1.2)
            height: units.gridUnit * 0.1
            opacity: control.hovered || down ? 1.0 : 0.0

            Behavior on color {
                ColorAnimation {
                    easing.type: Easing.Linear
                    duration: units.longDuration
                }
            }

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.Linear
                    duration: units.longDuration
                }
            }
        }
    }

    label: Item {
        implicitWidth: text.paintedWidth
        implicitHeight: text.paintedHeight
        baselineOffset: text.y + text.baselineOffset

        Text {
            id: text
            anchors.centerIn: parent
            renderType: Text.NativeRendering
            text: control.text
            font.bold: control.checkable && control.checked
            color: __syspal.buttonText
        }
    }
}
