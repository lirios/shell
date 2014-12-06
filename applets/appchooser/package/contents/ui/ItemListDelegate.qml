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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0 as KQuickControls
import Hawaii.Components 1.0

Item {
    width: ListView.view.width
    height: Math.max(icon.height, label.paintedHeight) + (units.smallSpacing * 2)

    RowLayout {
        anchors.fill: parent
        spacing: units.smallSpacing

/*
        Icon {
            id: icon
            width: units.iconSizes.large
            height: width
            iconName: model.iconName
        }
*/
        KQuickControls.QIconItem {
            id: icon
            width: units.iconSizes.large
            height: width
            icon: model.decoration
        }

        Column {
            Label {
                id: label
                text: model.display
                font.pointSize: theme.defaultFont.pointSize * 1.1
                font.bold: true
                color: PlasmaCore.ColorScope.textColor
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                textFormat: Text.PlainText
            }

            Label {
                id: descr
                text: model.description ? model.description : ""
                color: PlasmaCore.ColorScope.textColor
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                textFormat: Text.PlainText
                visible: text != ""
            }

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.fillWidth: true
        }

        LayoutMirroring.enabled: (Qt.application.layoutDirection == Qt.RightToLeft)
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: {
            parent.ListView.view.model.trigger(index, "", null);
            plasmoid.expanded = false;
        }
    }

    Accessible.role: Accessible.MenuItem
    Accessible.name: descr.visible ? label.text + "\n" + descr.text : label.text
}
