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

import QtQuick 2.0
import QtQuick.Controls 1.2
import "."

StackView {
    id: workspaces
    delegate: StackViewDelegate {
        function getTransition(properties) {
            return (__priv.direction === Qt.LeftToRight) ? ltrTransition : rtlTransition;
        }

        // Current workspace slide to the left, revealing next workspace
        property Component ltrTransition: StackViewTransition {
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: -target.width
                duration: __priv.duration
            }
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: target.width
                to: 0
                duration: __priv.duration
            }
        }

        // Current workspace slide to the right, reveling previous workspace
        property Component rtlTransition: StackViewTransition {
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: target.width
                duration: __priv.duration
            }
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: -target.width
                to: 0
                duration: __priv.duration
            }
        }
    }

    QtObject {
        id: __priv

        property int direction
        readonly property int duration: 500
    }

    Connections {
        target: Workspaces
        onWorkspaceAdded: function(workspace) {
            workspace.parent = workspaces;
        }
        onWorkspaceRemoved: function(index) {
            workspaces.pop({"item": Workspaces.get(index), "immediate": true});
        }
        onWorkspaceSwitched: function(index) {
            if (index > Workspaces.currentIndex)
                __priv.direction = Qt.LeftToRight;
            else if (index < Workspaces.currentIndex)
                __priv.direction = Qt.RightToLeft;
            else
                return;
            workspaces.push({"item": Workspaces.get(index)});
        }
    }
}
