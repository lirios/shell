// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Fluid.Controls 1.0 as FluidControls

Item {
    id: button

    property alias source: icon.source
    property alias color: icon.color

    signal clicked()

    width: icon.size
    height: width

    FluidControls.Icon {
        id: icon

        anchors.fill: parent
    }

    TapHandler {
        onTapped: {
            button.clicked();
        }
    }
}
