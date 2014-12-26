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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import ".."

Rectangle {
    property alias title: titleLabel.text
    property bool active: false

    signal close()
    signal minimize()
    signal maximize()

    id: root
    gradient: Gradient {
        GradientStop { color: Theme.window.primaryColor; position: 0 }
        GradientStop { color: Theme.window.secondaryColor; position: 1 }
    }

    RowLayout {
        anchors {
            fill: parent
            margins: units.gridUnit * 0.25
        }
        spacing: units.gridUnit
        opacity: root.active ? 1.0 : 0.5

        WindowControls {
            id: controls
            height: parent.height
            onClose: root.close();
            onMinimize: root.minimize();
            onMaximize: root.maximize();

            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            id: titleLabel
            color: Theme.window.textColor
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            style: Text.Raised
            styleColor: Theme.window.textEffectColor
            height: parent.height

            Layout.fillHeight: true
        }
    }
}
