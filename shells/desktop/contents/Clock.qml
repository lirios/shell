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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import ".."

ColumnLayout {
    property string timeFormat

    signal clicked()

    id: root

    PlasmaCore.DataSource {
        id: timeDataSource
        engine: "time"
        connectedSources: ["Local"]
        interval: 30000
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }

    Label {
        text: Qt.formatTime(timeDataSource.data["Local"]["DateTime"], timeFormat)
        font.pointSize: 36
        color: Theme.window.textColor

        Layout.alignment: Qt.AlignCenter
    }

    Label {
        text: Qt.formatDate(timeDataSource.data["Local"]["DateTime"], Locale.LongFormat)
        font.pointSize: 18
        color: Theme.window.textColor

        Layout.alignment: Qt.AlignCenter
    }

    Component.onCompleted: {
        // Remove seconds from time format
        timeFormat = Qt.locale().timeFormat(Locale.ShortFormat).replace(/.ss?/i, "");
    }
}
