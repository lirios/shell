/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0

/*!
    \qmltype Base
    \inqmlmodule Hawaii.Components.ListItems 1.0

    \brief Base class for list items.

    Provides an empty list item with the basic behavior.
*/
Item {
    id: root

    property alias text: label.text

    /*!
        True if the user presses a mouse button in the item's area.
    */
    property bool pressed: mouseArea.pressed

    /*!
        True if the mouse pointer hovers the item's area.
    */
    property bool hovered: __acceptEvents && mouseArea.containsMouse

    /*!
        Show or hide the progression symbol.
    */
    property bool progression: false

    /*!
        Specifies whether the list item is selected or not.
    */
    property bool selected: false

    /*!
        Highlight the list item when it's pressed.
        Disable to implement a custom highlighting.
    */
    property bool highlightWhenPressed: true

    /*!
        Defines whether this item can be removed or not.
    */
    property bool removable: false

    /*!
        Show or hide a thin bottom separator line (drawn by the \l ThinSeparator component).
    */
    property bool showSeparator: true

    /*!
        \internal
        Avoid occluding the separator with children visuals.
    */
    default property alias children: body.data

    /*!
        \internal
        Allow derived list items to add items inside of this element.
    */
    property alias __contents: body

    /*!
        \internal
        Whether mouse events are propagated or not.
        Some list items may want to handle mouse themselves.
    */
    property bool __acceptEvents: true

    /*!
        \internal
        Palette.
    */
    property var __syspal: SystemPalette {
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    /*!
        Emitted when there is a mouse click on the item and the item
        is not disabled.
    */
    signal clicked()

    /*!
        Emitted when a mouse button is pressed and hold and the
        item is not disabled.
    */
    signal pressAndHold()

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        onClicked: if (root.__acceptEvents) root.clicked()
        onPressAndHold: if (root.__acceptEvents) root.pressAndHold()
    }

    Rectangle {
        id: highlight
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        z: -1
        height: root.height - separator.height
        color: __syspal.highlight
    }

    ThinSeparator {
        id: separator
        anchors.bottom: parent.bottom
        visible: showSeparator
    }

    Accessible.role: Accessible.Button
}
