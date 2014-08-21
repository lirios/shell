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
import Hawaii.Shell.Controls 1.0 as Controls
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.hawaii.appchooser.private 0.1 as AppChooser

FocusScope {
    property alias model: listView.model

    implicitWidth: units.gridUnit * 10
    implicitHeight: listView.contentHeight

    AppChooser.AppsModel {
        id: appsModel

        flat: true
        entryPath: "/"
        appNameFormat: 0
        appletInterface: plasmoid
    }

    Controls.ScrollView {
        anchors.fill: parent
        focus: true

        ListView {
            id: listView
            boundsBehavior: Flickable.StopAtBounds
            snapMode: ListView.SnapToItem
            model: appsModel
            delegate: ItemListDelegate {}
            onModelChanged: {
                if (model == null)
                    model = appsModel
            }
        }
    }
}
