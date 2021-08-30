// SPDX-FileCopyrightText: 2020 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Fluid.Controls 1.0 as FluidControls
import Liri.Shell 1.0 as Shell

Shell.Indicator {
    title: qsTr("Shut down")
    iconSource: FluidControls.Utils.iconUrl("action/power_settings_new")
}
