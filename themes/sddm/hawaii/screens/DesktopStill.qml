/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
import QtGraphicalEffects 1.0
import Hawaii.Themes 1.0 as Themes
import Hawaii.Controls 1.0 as Controls
import Hawaii.GSettings 1.0 as Settings

Item {
    property int stage: 0

    id: root
    onStageChanged: {
        if (stage == 6)
            desaturate.desaturation = 0.4;
    }

    Settings.Settings {
        id: bgSettings
        schema.id: "org.hawaiios.desktop.background"
        schema.path: "/org/hawaiios/desktop/background/"
    }


    // TODO: Load a copy of user selecter wallpaper with blur
    // and remove the FastBlur here
    Image {
        id: image
        anchors.fill: parent
        source: bgSettings.pictureUrl
        sourceSize.width: width
        sourceSize.height: height
        smooth: true
        visible: false
    }
}
