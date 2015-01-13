import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "."

Item {
    signal unlocked()

    id: root
    opacity: 0.0

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: units.longDuration
        }
    }

    QtObject {
        id: __priv

        property string timeFormat
    }

    Image {
        id: picture
        anchors.fill: parent
        source: "/usr/share/backgrounds/gnome/Stones.jpg"
        sourceSize.width: width * 0.75
        sourceSize.height: height * 0.75
        fillMode: Image.PreserveAspectCrop
        cache: false
        visible: false
    }

    FastBlur {
        anchors.fill: picture
        source: picture
        radius: 32
    }

    PlasmaCore.DataSource {
        id: timeDataSource
        engine: "time"
        connectedSources: ["Local"]
        interval: 5000
    }

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
        }

        Label {
            text: Qt.formatTime(timeDataSource.data["Local"]["DateTime"], __priv.timeFormat)
            font.pointSize: 42
            color: Theme.palette.window.textColor
            style: Text.Raised
            styleColor: Theme.palette.window.textEffectColor

            Layout.alignment: Qt.AlignCenter
        }

        Label {
            text: Qt.formatDate(timeDataSource.data["Local"]["DateTime"], Locale.LongFormat)
            font.pointSize: 36
            color: Theme.palette.window.textColor
            style: Text.Raised
            styleColor: Theme.palette.window.textEffectColor

            Layout.alignment: Qt.AlignCenter
        }

        Item {
            Layout.fillHeight: true
        }

        Rectangle {
            color: "#80000000"

            TextField {
                id: passwordField
                anchors.centerIn: parent
                placeholderText: qsTr("Password")
                width: units.gridUnit * 20
                focus: true
                echoMode: TextInput.Password
                onAccepted: root.unlocked()
            }

            Layout.fillWidth: true
            Layout.preferredHeight: units.gridUnit * 5
        }

        Item {
            Layout.fillHeight: true
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onClicked: passwordField.forceActiveFocus()
    }

    Component.onCompleted: {
        // Remove seconds from time format
        __priv.timeFormat = Qt.locale().timeFormat(Locale.ShortFormat).replace(/.ss?/i, "");

        // Trigger opacity animation
        opacity = 1.0;

        // Activate password field
        passwordField.forceActiveFocus();
    }
}
