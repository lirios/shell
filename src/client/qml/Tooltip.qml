/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Layouts 1.0
import Hawaii.Shell 0.2
import Hawaii.Shell.Styles 0.1

TooltipWindow {
    property alias text: label.text

    StyledItem {
        id: styledItem
        style: Qt.createComponent("TooltipStyle.qml", styledItem)
        width: mainLayout.implicitWidth + __style.padding.left + __style.padding.right
        height: mainLayout.implicitHeight + __style.padding.top + __style.padding.bottom

        ColumnLayout {
            id: mainLayout
            anchors {
                fill: parent
                leftMargin: __style.padding.left
                topMargin: __style.padding.top
                rightMargin: __style.padding.right
                bottomMargin: __style.padding.bottom
            }

            Text {
                id: label
                color: styledItem.__style.textColor
                style: Text.Raised
                styleColor: styledItem.__style.textShadowColor
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 4
                wrapMode: Text.Wrap

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.maximumWidth: 400
                Layout.fillHeight: true
            }
        }
    }
}
