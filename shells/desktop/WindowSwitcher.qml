import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import GreenIsland 1.0
import GreenIsland.Core 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "."

Rectangle {
    property bool active: false

    id: root
    color: "#80000000"
    radius: units.gridUnit * 0.5
    opacity: active ? 1.0 : 0.0
    states: [
        State {
            name: "active"
            when: active

            StateChangeScript {
                name: "disableInput"
                script: {
                    var i;
                    for (i = 0; i < compositorRoot.surfaceModel.count; i++) {
                        var window = compositorRoot.surfaceModel.get(i).window;
                        window.child.focus = false;
                    }
                }
            }
        },
        State {
            name: "inactive"
            when: !active
        }
    ]

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: units.shortDuration
        }
    }

    // Keyboard event handling
    Connections {
        target: compositorRoot
        onKeyPressed: {
            // Do not even begin if there are no windows
            if (!active && listView.model.count < 2)
                return;

            if (event.modifiers & Qt.MetaModifier) {
                if (event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab) {
                    // Activate window switcher
                    root.active = true;
                    event.accepted = true;
                }
            }
        }
        onKeyReleased: {
            // Do not even begin if there are no windows
            if (!active && listView.model.count < 2)
                return;

            if (event.modifiers & Qt.MetaModifier) {
                if (event.key === Qt.Key_Tab) {
                    // Switch between windows
                    if (listView.currentIndex == listView.count - 1)
                        listView.currentIndex = 0;
                    else
                        listView.currentIndex++;
                } else if (event.key === Qt.Key_Backtab) {
                    // Activate window switcher
                    root.active = true;

                    // Switch between windows
                    if (listView.currentIndex == 0)
                        listView.currentIndex = listView.count - 1;
                    else
                        listView.currentIndex--;
                }
            } else {
                // Keys released, deactivate switcher
                root.active = false;

                // Give focus to the selected window
                var item = compositorRoot.surfaceModel.get(listView.currentIndex);
                if (item !== undefined)
                    compositorRoot.moveFront(item.window);
            }

            event.accepted = true;
        }
    }

    ListView {
        readonly property real ratio: compositorRoot.width / compositorRoot.height

        id: listView
        anchors {
            fill: parent
            margins: units.largeSpacing
        }
        clip: true
        orientation: ListView.Horizontal
        model: compositorRoot.surfaceModel
        spacing: units.smallSpacing
        currentIndex: compositorRoot.activeWindowIndex
        highlightMoveDuration: units.shortDuration
        delegate: Item {
            readonly property real scaleFactor: listView.width / compositorRoot.width
            readonly property real thumbnailWidth: thumbnailHeight * listView.ratio
            readonly property real thumbnailHeight: listView.height - units.smallSpacing - (units.largeSpacing * 2)

            id: wrapper
            width: thumbnailWidth + thumbnailLayout.anchors.margins + thumbnailLayout.spacing
            height: thumbnailHeight + thumbnailLayout.anchors.margins + thumbnailLayout.spacing
            visible: model.surface.windowType === QuickSurface.Toplevel

            ColumnLayout {
                id: thumbnailLayout
                anchors {
                    fill: parent
                    margins: units.smallSpacing
                }
                spacing: units.largeSpacing

                Rectangle {
                    id: thumbnailItem
                    color: wrapper.ListView.isCurrentItem ? Theme.panel.selectedBackgroundColor : "transparent"
                    radius: units.gridUnit * 0.5
                    width: thumbnailWidth - units.smallSpacing
                    height: thumbnailHeight - units.smallSpacing - label.height

                    SurfaceRenderer {
                        anchors {
                            fill: parent
                            margins: units.smallSpacing
                        }
                        source: model.window.child

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.AllButtons
                            onClicked: listView.currentIndex = index
                        }
                    }
                }

                Label {
                    id: label
                    text: model.surface.title ? model.surface.title : qsTr("Untitled")
                    wrapMode: Text.Wrap
                    color: Theme.panel.textColor
                    font.bold: true
                    style: Text.Raised
                    styleColor: Theme.panel.textEffectColor
                    maximumLineCount: 2
                    opacity: wrapper.ListView.isCurrentItem ? 1.0 : 0.6

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
