/****************************************************************************
 * This file is part of Kahai.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import "indicators"

Rectangle {
    property Indicator selectedIndicator: null

    signal indicatorTriggered(var indicator)

    color: "transparent"
    height: units.iconSizes.large

    RowLayout {
        anchors.fill: parent

        RowLayout {
            id: leftContainer
            spacing: units.largeSpacing

            Indicator {
                text: qsTr("Menu")
            }

            Layout.alignment: Qt.AlignLeft
        }

        Launcher {
            id: launcher

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            id: indicators
            spacing: units.largeSpacing

            ChatIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            EventsIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            SettingsIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            VolumeIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            NetworkIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            BatteryIndicator {
                onTriggered: indicatorTriggered(caller)
            }

            Layout.alignment: Qt.AlignRight
        }
    }
}
