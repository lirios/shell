/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
import Hawaii.Themes 1.0 as Themes
import org.hawaii.launcher 0.1 as CppLauncher
import "../components"

FocusScope {
    signal closed()

    id: root
    //height: Themes.Units.gu(17)

    CppLauncher.ProcessRunner {
        id: processRunner

        function executeSetting(name) {
            root.closed();
            launchApplication("org.hawaii.systempreferences." + name);
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Themes.Units.largeSpacing

        RowLayout {
            spacing: Themes.Units.largeSpacing

            RowLayout {
                spacing: Themes.Units.largeSpacing

                Label {
                    text: "Weather: 20 C, Clear"
                    font.pointSize: Themes.Theme.defaultFont.pointSize * 0.9
                    color: Themes.Theme.palette.panel.textColor

                    MouseArea {
                        anchors.fill: parent
                        onClicked: stackView.push(weatherComponent)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "Clock Settings"
                    //onClicked: stackView.push(clockSettingsComponent)
                    onClicked: processRunner.executeSetting("clock")
                }

                Layout.alignment: Qt.AlignTop
            }

            Clock {
                onClicked: stackView.push(calendarComponent)
                onDoubleClicked: root.closed()

                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: true
            }

            RowLayout {
                spacing: Themes.Units.largeSpacing

                Button {
                    text: "Add Appointment"
                    onClicked: stackView.push(appointmentComponent)
                }

                Item {
                    Layout.fillWidth: true
                }

                Label {
                    text: "System Language: English"
                    font.pointSize: Themes.Theme.defaultFont.pointSize * 0.9
                    color: Themes.Theme.palette.panel.textColor

                    MouseArea {
                        anchors.fill: parent
                        //onClicked: stackView.push(languageComponent)
                        onClicked: processRunner.executeSetting("translations")
                    }
                }

                Layout.alignment: Qt.AlignTop
            }
        }

        StackView {
            id: stackView
            initialItem: calendarComponent

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Component {
        id: calendarComponent

        CalendarView {
            selectedDate: new Date()
        }
    }

    Component {
        id: weatherComponent

        Item {
            Label {
                anchors.centerIn: parent
                text: "Not implemented yet"
                color: Themes.Theme.palette.panel.textColor
            }
        }
    }

    Component {
        id: clockSettingsComponent

        Item {
            Label {
                anchors.centerIn: parent
                text: "Not implemented yet"
                color: Themes.Theme.palette.panel.textColor
            }
        }
    }

    Component {
        id: appointmentComponent

        Item {
            Label {
                anchors.centerIn: parent
                text: "Not implemented yet"
                color: Themes.Theme.palette.panel.textColor
            }
        }
    }

    Component {
        id: languageComponent

        Item {
            Label {
                anchors.centerIn: parent
                text: "Not implemented yet"
                color: Themes.Theme.palette.panel.textColor
            }
        }
    }
}
