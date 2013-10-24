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

Item {
    property var authenticationDialog: AuthenticationDialog {}
    property var shutdownDialog: ShutdownDialog {}

    Connections {
        id: polkitAgentConnections
        target: null
        onAuthenticationInitiated: {
            // Fill the properties and initialize
            authenticationDialog.actionId = actionId;
            authenticationDialog.message = message;
            authenticationDialog.iconName = iconName;
            authenticationDialog.realName = realName;
            authenticationDialog.avatar = avatar;
            authenticationDialog.initialize();
        }
        onAuthenticationRequested: {
            // Change authentication prompt and echo mode
            authenticationDialog.prompt = prompt;
            authenticationDialog.echo = echo;

            // Show the dialog
            authenticationDialog.visible = true;
        }
        onAuthorized: authenticationDialog.visible = false
        onAuthenticationCanceled: authenticationDialog.visible = false
        onInfoMessage: authenticationDialog.infoMessage = message
        onErrorMessage: authenticationDialog.errorMessage = message
    }

    Connections {
        id: polkitDialogConnections
        target: authenticationDialog
        onAuthenticationReady: polkitAgentConnections.target.authenticate(response)
        onAuthenticationCanceled: polkitAgentConnections.target.abortAuthentication()
    }

    function register() {
        polkitAgentConnections.target = Shell.service("PolicyKitAgent");
    }
}
