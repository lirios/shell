/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0
import Liri.Storage 1.0 as Storage

Indicator {
    title: qsTr("Storage")
    iconSource: Qt.resolvedUrl("../images/harddisk.svg")
    visible: storageModel.count > 0
    component: ListView {
        model: storageModel
        clip: true

        delegate: FluidControls.ListItem {
            icon.name: model.iconName + "-symbolic"
            text: model.name
            onClicked: processRunner.launchCommand("xdg-open file://" + model.filePath)

            rightItem: ToolButton {
                anchors.centerIn: parent

                ToolTip.text: model.mounted ? qsTr("Eject") : qsTr("Mount")
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.visible: hovered

                icon.source: Qt.resolvedUrl("../images/" + (model.mounted ? "eject.svg" : "disk.svg"))
                onClicked: model.mounted ? model.storageDevice.unmount() : model.storageDevice.mount()
            }
        }
    }

    Storage.StorageModel {
        id: storageModel
    }
}
