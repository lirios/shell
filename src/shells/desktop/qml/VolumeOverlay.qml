/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layout 1.0
import Hawaii.Shell.Desktop 0.1
import Hawaii.Shell.Styles 0.1
import FluidUi 0.2 as FluidUi

Overlay {
    width: 400
    height: 400

    ColumnLayout {
        anchors.fill: parent

        FluidUi.Icon {
            iconName: "audio-volume-high-symbolic"
            color: "white"
        }

        ProgressBar {
            minimumValue: 0.5
            maximumValue: 1.0

            Layout.fillWidth: true
        }
    }
}
