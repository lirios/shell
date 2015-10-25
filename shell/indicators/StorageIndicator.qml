/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import Hawaii.Controls 1.0 as Controls
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaiios.hardware 0.1 as Hardware
import ".."

Indicator {
    name: "storage"
    iconName: "drive-harddisk-symbolic"
    component: Component {
        ColumnLayout {
            spacing: Themes.Units.largeSpacing

            Controls.Heading {
                text: qsTr("Volumes")
                color: Themes.Theme.palette.panel.textColor
            }

            Repeater {
                model: hardware.storageDevices

                RowLayout {
                    spacing: Themes.Units.smallSpacing

                    RowLayout {
                        spacing: Themes.Units.smallSpacing

                        Components.Icon {
                            iconName: modelData.iconName + "-symbolic"
                            width: Themes.Units.iconSizes.large
                            height: width
                            color: Themes.Theme.palette.panel.textColor

                            MouseArea {
                                anchors.fill: parent
                                onClicked: Qt.openUrlExternally(modelData.filePath)
                            }
                        }

                        Text {
                            renderType: Text.NativeRendering
                            text: modelData.name
                            color: Themes.Theme.palette.panel.textColor

                            MouseArea {
                                anchors.fill: parent
                                onClicked: Qt.openUrlExternally(modelData.filePath)
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        ToolButton {
                            iconName: modelData.mounted ? "media-eject-symbolic" : "media-removable-symbolic"
                            onClicked: modelData.mounted ? modelData.unmount() : modelData.mount()
                        }
                    }

                    Layout.fillWidth: true
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
    visible: hardware.storageDevices.length > 0

    Hardware.HardwareEngine {
        id: hardware
    }
}
