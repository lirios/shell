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
    QtObject {
        id: __priv

        property var authenticationDialog: null
    }

    Connections {
        id: polkitAgentConnections
        target: null
        onAuthenticationInitiated: {
            // Create the dialog
            var component = Qt.createComponent(Qt.resolvedUrl("AuthenticationDialog.qml"));
            __priv.authenticationDialog = component.createObject();

            // Fill the properties
            __priv.authenticationDialog.actionId = actionId;
            __priv.authenticationDialog.message = message;
            __priv.authenticationDialog.iconName = iconName;
            __priv.authenticationDialog.realName = realName;
            __priv.authenticationDialog.avatar = avatar;
        }
        onAuthenticationRequested: {
            // Change authentication prompt and echo mode
            __priv.authenticationDialog.prompt = prompt;
            __priv.authenticationDialog.echo = echo;

            // Show the dialog
            __priv.authenticationDialog.visible = true;
        }
        onAuthorized: __priv.authenticationDialog.accept()
        onAuthenticationCanceled: {console.log("****CANC***");__priv.authenticationDialog.reject();}
        onInfoMessage: {
            if (__priv.authenticationDialog)
                __priv.authenticationDialog.infoMessage = message
        }
        onErrorMessage: {
            if (__priv.authenticationDialog)
                __priv.authenticationDialog.errorMessage = message;
        }
    }

    Connections {
        id: polkitDialogConnections
        target: __priv.authenticationDialog
        onAuthenticationReady: polkitAgentConnections.target.authenticate(response)
        onAuthenticationCanceled: polkitAgentConnections.target.abortAuthentication()
        onAccepted: __priv.authenticationDialog = null
        onRejected: {console.log("***REJ****");__priv.authenticationDialog = null;}
    }

    function showShutdownDialog() {
        var component = Qt.createComponent(Qt.resolvedUrl("ShutdownDialog.qml"));
        var dialog = component.createObject();
        dialog.visible = true;
    }

    function register() {
        polkitAgentConnections.target = Shell.service("PolicyKitAgent");
    }
}
