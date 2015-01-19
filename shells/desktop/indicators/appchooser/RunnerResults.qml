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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import Hawaii.Controls 1.0 as Controls
import Hawaii.Themes 1.0 as Themes
import org.kde.plasma.core 2.0 as PlasmaCore

ColumnLayout {
    implicitHeight: label.paintedHeight + horizLine.height + runnerMatches.contentHeight + (spacing * 2)
    spacing: Themes.Units.smallSpacing

    Controls.Heading {
        id: label
        level: 3
        elide: Text.ElideRight
        text: (runnerMatches.model != null) ? runnerMatches.model.name : ""
        color: Themes.Theme.palette.panel.textColor

        Layout.alignment: Qt.AlignCenter
    }

    PlasmaCore.SvgItem {
        id: horizLine
        svg: lineSvg
        elementId: "horizontal-line"
        height: Themes.Units.gridUnit

        Layout.fillWidth: true
    }

    ListView {
        id: runnerMatches
        model: runnerModel.modelForRow(index)
        delegate: ItemListDelegate {}
        onModelChanged: {
            if (model == undefined || model == null) {
                enabled = false;
                visible = false;
            }
        }

        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
