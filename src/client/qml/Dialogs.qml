/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

pragma Singleton

import QtQuick 2.0

QtObject {
    AuthenticationDialog {
        id: authenticationDialog
    }

    ShutdownDialog {
        id: shutdownDialog
    }

    Connections {
        target: Shell.service("PolicyKitAgent")
        onAuthenticationInitiated: {
            // Hide the dialog
            authenticationDialog.visible = false;

            // Fill the properties
            authenticationDialog.actionId = actionId;
            authenticationDialog.message = message;
            authenticationDialog.iconName = iconName;
            authenticationDialog.details = details;
            //authenticationDialog.identities = identities;
            authenticationDialog.realName = realName;
            authenticationDialog.avatar = avatar;

            // Reset the UI
            authenticationDialog.echo = false;
            authenticationDialog.response = "";
            authenticationDialog.infoMessage = "";
            authenticationDialog.errorMessage = "";
        }
        onAuthenticationRequested: {
            // Change authentication prompt and echo mode
            authenticationDialog.prompt = prompt;
            authenticationDialog.echo = echo;

            // And show the dialog
            authenticationDialog.visible = true;
        }
        onAuthenticationFinished: authenticationDialog.visible = false
        onAuthenticationCanceled: authenticationDialog.visible = false
        onInfoMessage: authenticationDialog.infoMessage = message
        onErrorMessage: authenticationDialog.errorMessage = message
    }

    Component.onCompleted: console.log("************SINGLE*****")
}
