// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import Fluid.Controls 1.0 as FluidControls
import QtAccountsService 1.0 as Accounts
import Liri.Shell 1.0 as Shell

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
