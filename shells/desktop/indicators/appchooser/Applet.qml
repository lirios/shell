/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import Hawaii.Themes 1.0 as Themes
import org.hawaii.appchooser 0.1 as AppChooser

Item {
    id: root

    AppChooser.ProcessRunner {
        id: processRunner

        function executeUserSettings() {
            if (indicator.expanded)
                indicator.triggered(indicator);
            runUserSettings();
        }
    }

    AppChooser.RunnerModel {
        id: runnerModel
        runners: new Array("bookmarks", "baloosearch")
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: Themes.Units.smallSpacing
        spacing: Themes.Units.largeSpacing

        Header {
            Layout.fillWidth: true
        }

        Item {
            Loader {
                anchors.fill: parent
                active: runnerModel.count == 0
                sourceComponent: VerticalView {}
            }

            Loader {
                anchors.fill: parent
                active: (searchField !== "") && (runnerModel.count > 0)
                sourceComponent: ScrollView {
                    ListView {
                        id: runnerView
                        model: runnerModel
                        delegate: RunnerResults {
                            id: runnerMatches
                            width: runnerView.width
                        }

                        Connections {
                            target: runnerModel
                            onRunnersChanged: runnerView.forceLayout()
                            onCountChanged: runnerView.forceLayout()
                        }
                    }
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            ExclusiveGroup {
                id: modeGroup
            }

            TextField {
                id: searchField
                placeholderText: qsTr("Search...")
                focus: true
                //clearButtonShown: true
                onTextChanged: runnerModel.query = text

                Layout.fillWidth: true
            }

            Layout.fillWidth: true
        }

        ShutdownActions {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        }
    }
}
