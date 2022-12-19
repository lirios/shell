/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2014-2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.5

Loader {
    id: inputPanel

    readonly property bool keyboardAvailable: status === Loader.Ready
    readonly property bool keyboardActive: item ? item.active : false

    function toggle() {
        state = state == "hidden" ? "visible" : "hidden";
    }

    source: "components/VirtualKeyboard.qml"

    onKeyboardActiveChanged: {
        if (keyboardActive)
            state = "visible";
        else
            state = "hidden";
    }

    state: "hidden"
    states: [
        State {
            name: "visible"
            PropertyChanges {
                target: greeter
                height: root.height - inputPanel.height
            }
            PropertyChanges {
                target: inputPanel
                y: root.height - inputPanel.height
                opacity: 1.0
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: greeter
                height: root.height
            }
            PropertyChanges {
                target: inputPanel
                y: root.height
                opacity: 0.0
            }
        }
    ]
    transitions: [
        Transition {
            from: "hidden"
            to: "visible"
            SequentialAnimation {
                ScriptAction {
                    script: {
                        inputPanel.item.activated = true;
                        Qt.inputMethod.show();
                    }
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: greeter
                        property: "height"
                        duration: 250
                        easing.type: Easing.OutQuad
                    }
                    YAnimator {
                        target: inputPanel
                        duration: 250
                        easing.type: Easing.OutQuad
                    }
                    OpacityAnimator {
                        target: inputPanel
                        duration: 250
                        easing.type: Easing.OutQuad
                    }
                }
            }
        },
        Transition {
            from: "visible"
            to: "hidden"
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: greeter
                        property: "height"
                        duration: 250
                        easing.type: Easing.OutQuad
                    }
                    YAnimator {
                        target: inputPanel
                        duration: 250
                        easing.type: Easing.InQuad
                    }
                    OpacityAnimator {
                        target: inputPanel
                        duration: 250
                        easing.type: Easing.InQuad
                    }
                }
                ScriptAction {
                    script: {
                        Qt.inputMethod.hide();
                    }
                }
            }
        }
    ]
}
