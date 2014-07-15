/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import Hawaii.Configuration 1.0
import Hawaii.SystemPreferences.Background 1.0

Item {
    id: root

    Configuration {
        id: settings
        category: "shell/backgrounds/org.hawaii.backgrounds.gradient"

        property string type: "vertical"
        property color primaryColor: "#336699"
        property color secondaryColor: "#334455"
    }

    Configuration {
        id: shellSettings
        category: "shell"

        property string background
    }

    RowLayout {
        anchors.fill: parent

        ComboBox {
            model: [
                qsTr("Horizontal"),
                qsTr("Vertical")
            ]
            onActivated: {
                settings.type = index == 0 ? "horizontal" : "vertical";
                shellSettings.background = "org.hawaii.backgrounds.gradient";
            }

            Layout.minimumWidth: 100
        }

        ColorButton {
            id: colorButton1
            color: settings.primaryColor
            onColorChanged: {
                settings.primaryColor = color;
                shellSettings.background = "org.hawaii.backgrounds.gradient";
            }
        }

        ColorButton {
            id: colorButton2
            color: settings.secondaryColor
            onColorChanged: {
                settings.secondaryColor = color;
                shellSettings.background = "org.hawaii.backgrounds.gradient";
            }
        }
    }
}
