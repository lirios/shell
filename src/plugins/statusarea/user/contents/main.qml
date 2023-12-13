// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import Fluid.Controls as FluidControls
import QtAccountsService as Accounts
import Liri.Shell as Shell

Shell.StatusAreaExtension {
    Accounts.UserAccount {
        id: currentUser
    }

    indicator: Shell.Indicator {
        title: currentUser.displayName

        FluidControls.CircleImage {
            anchors.centerIn: parent
            source: "file://" + currentUser.iconFileName
            width: iconSize
            height: iconSize
        }
    }
}
