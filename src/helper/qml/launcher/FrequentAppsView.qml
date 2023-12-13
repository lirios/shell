// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQml.Models
import Fluid.Controls as FluidControls
import Liri.Shell.TaskManager as TaskManager

Item {
    id: frequentApps

    readonly property alias count: repeater.count

    width: 130 * grid.columns
    height: 130 * grid.rows

    FluidControls.Placeholder {
        anchors.fill: parent

        icon.source: FluidControls.Utils.iconUrl("action/history")
        text: qsTr("Frequent Apps")
        subText: qsTr("The apps you use frequently will show here")
        visible: frequentApps.count === 0
    }

    Grid {
        id: grid

        anchors.fill: parent

        columns: 6
        rows: 2

        Repeater {
            id: repeater

            model: TaskManager.PageModel {
                limitCount: grid.rows * grid.columns
                sourceModel: TaskManager.FrequentAppsModel {
                    id: frequentAppsModel

                    sourceModel: appsModel
                }
            }

            delegate: Item {

                width: 130
                height: width

                AppsGridDelegate {
                    id: delegate

                    anchors {
                        fill: parent
                        margins: FluidControls.Units.smallSpacing
                    }
                }
            }
        }
    }
}
