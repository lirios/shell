/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import Hawaii.Components 1.0 as Components
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import org.kde.kcoreaddons 1.0 as KCoreAddons

RowLayout {
    spacing: Themes.Units.largeSpacing

    KCoreAddons.KUser {
        id: user
        onFaceIconUrlChanged: setFaceIcon()
    }

    Components.Icon {
        id: faceIcon
        width: Themes.Units.iconSizes.large
        height: width
        cache: false
    }

    Controls.Heading {
        level: 2
        text: user.fullName
        color: Themes.Theme.palette.panel.textColor

        Layout.alignment: Qt.AlignVCenter
        Layout.fillWidth: true
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: processRunner.executeUserSettings()
    }

    function setFaceIcon() {
        if (user.faceIconUrl != "") {
            // This is a picture from the file system
            faceIcon.iconSource = user.faceIconUrl;
            faceIcon.iconName = "";
            faceIcon.color = Qt.rgba(0, 0, 0, 0);
        } else {
            // Fallback to an icon from the theme
            faceIcon.iconSource = "";
            faceIcon.iconName = "avatar-default-symbolic";
            faceIcon.color = Themes.Theme.palette.panel.textColor;
        }
    }

    Component.onCompleted: setFaceIcon()
}
