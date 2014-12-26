import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "."

Item {
    id: root
    enabled: false
    opacity: enabled ? 1.0 : 0.0

    QtObject {
        id: __priv

        property string timeFormat
    }

    Connections {
        target: compositorRoot
        onKeyPressed: {
            if (enabled)
                return;

            // Trigger lock screen
            if (event.modifiers & Qt.MetaModifier && event.key === Qt.Key_L) {
                enabled = true;
                event.accepted = true;
            }
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InSine
            duration: units.longDuration
        }
    }

    Image {
        id: picture
        anchors.fill: parent
        source: "/usr/share/wallpapers/Green_Leaves/contents/images/1920x1080.png"
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectCrop
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
        interval: root.enabled ? 30000 : 0
    }

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
        }

        Label {
            text: Qt.formatTime(timeDataSource.data["Local"]["DateTime"], __priv.timeFormat)
            font.pointSize: 42
            color: Theme.window.textColor
            style: Text.Raised
            styleColor: Theme.window.textEffectColor

            Layout.alignment: Qt.AlignCenter
        }

        Label {
            text: Qt.formatDate(timeDataSource.data["Local"]["DateTime"], Locale.LongFormat)
            font.pointSize: 36
            color: Theme.window.textColor
            style: Text.Raised
            styleColor: Theme.window.textEffectColor

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
                focus: root.enabled
                echoMode: TextInput.Password
                onAccepted: root.enabled = false
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
        enabled: root.enabled
        onClicked: passwordField.forceActiveFocus()
    }

    Component.onCompleted: {
        // Remove seconds from time format
        __priv.timeFormat = Qt.locale().timeFormat(Locale.ShortFormat).replace(/.ss?/i, "");
    }
}
