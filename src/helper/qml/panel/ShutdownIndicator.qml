// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Fluid.Controls as FluidControls
import Liri.Shell as Shell

Shell.Indicator {
    title: qsTr("Shut down")
    iconSource: FluidControls.Utils.iconUrl("action/power_settings_new")
}
