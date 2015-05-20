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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import SddmComponents 2.0 as SddmComponents
import Hawaii.Themes 1.0 as Themes
import Hawaii.Components 1.0 as Components
import Hawaii.Components.ListItems 1.0 as ListItems

Components.NoiseBackground {
    property var usersModel
    property int sessionIndex: sessionModel.lastIndex
    property alias rebootVisible: shutdownOptions.rebootVisible
    property alias powerOffVisible: shutdownOptions.powerOffVisible

    signal loginRequested(string userName, string password, int sessionIndex)
    signal rebootRequested()
    signal powerOffRequested()

    id: greeter
    color: "#272727"
    gradient: Gradient {
        GradientStop { position: 0; color: "#272727" }
        GradientStop { position: 1; color: "#2b2b2b" }
    }

    Connections {
        target: sddm
        onLoginFailed: {
            loginScreen.setErrorMessage(qsTr("Login Failed"));
            loginScreen.clearPassword();
        }
    }

    Item {
        anchors {
            centerIn: parent
            bottomMargin: shutdownOptionsWrapper.height * Themes.Units.smallSpacing
        }
        width: mainLayout.implicitWidth
        height: mainLayout.implicitHeight

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent

            StackView {
                id: stackView
                width: greeter.width * 0.5
                height: greeter.height * 0.5
                initialItem: LoginScreen {
                    id: loginScreen
                    width: greeter.width * 0.5
                    height: Themes.Units.largeSpacing * 14
                    actions: Row {
                        spacing: Themes.Units.smallSpacing

                        //KeyboardLayoutButton {}
                        SessionButton {}
                    }
                    model: greeter.usersModel
                    onLoginRequested: greeter.loginRequested(userName, password, greeter.sessionIndex)
                }
            }
        }
    }

    Item {
        id: shutdownOptionsWrapper
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: Themes.Units.largeSpacing
        }
        width: shutdownOptions.implicitWidth
        height: shutdownOptions.implicitHeight

        ShutdownButtons {
            id: shutdownOptions
            anchors.fill: parent
            spacing: Themes.Units.largeSpacing
            onRebootRequested: greeter.rebootRequested()
            onPowerOffRequested: greeter.powerOffRequested()
        }
    }
}
