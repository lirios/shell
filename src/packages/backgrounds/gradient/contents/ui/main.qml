/****************************************************************************
 * This file is part of Hawaii Shell.
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
import Fluid.Ui 1.0 as FluidUi
import Hawaii.Configuration 1.0

Item {
    id: background

    GradientItem {
        id: vgradient
        anchors.fill: parent
        visible: false
    }

    GradientItem {
        id: hgradient
        anchors.fill: parent
        rotation: 270
        scale: 2
        visible: false
    }

    Configuration {
        id: settings
        category: "shell/backgrounds/org.hawaii.backgrounds.gradient"

        property string type: "vertical"
        property color primaryColor: "#336699"
        property color secondaryColor: "#334455"

        function applyChanges() {
            if (type == "vertical") {
                vgradient.primaryColor = primaryColor;
                vgradient.secondaryColor = secondaryColor;
                hgradient.visible = false;
                vgradient.visible = true;
            } else {
                hgradient.primaryColor = primaryColor;
                hgradient.secondaryColor = secondaryColor;
                vgradient.visible = false;
                hgradient.visible = true;
            }
        }

        Component.onCompleted: {
            applyChanges();
            settings.onTypeChanged.connect(applyChanges);
            settings.onPrimaryColorChanged.connect(applyChanges);
            settings.onSecondaryColorChanged.connect(applyChanges);
        }
    }
}
