import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import GreenIsland 1.0
import Hawaii.Themes 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import "."

Rectangle {
    id: root
    color: "#80000000"
    radius: units.gridUnit * 0.5
    opacity: 0.0

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InQuad
            duration: units.shortDuration
        }
    }

    // Keyboard event handling
    Connections {
        target: compositorRoot
        onWindowSwitchPrev: {
            if (listView.currentIndex == 0)
                listView.currentIndex = listView.count - 1;
            else
                listView.currentIndex--;
        }
        onWindowSwitchNext: {
            if (listView.currentIndex == listView.count - 1)
                listView.currentIndex = 0;
            else
                listView.currentIndex++;
        }
        onWindowSwitchSelect: {
            // Give focus to the selected window
            /*
            var item = compositorRoot.surfaceModel.get(listView.currentIndex);
            if (item !== undefined)
                compositorRoot.moveFront(item.window);
                */
            compositorRoot.moveFront(compositorRoot.windowList[listView.currentIndex]);
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
        model: compositorRoot.windowList
        spacing: units.smallSpacing
        highlightMoveDuration: units.shortDuration
        delegate: Item {
            readonly property real scaleFactor: listView.width / compositorRoot.width
            readonly property real thumbnailWidth: thumbnailHeight * listView.ratio
            readonly property real thumbnailHeight: listView.height - units.smallSpacing - (units.largeSpacing * 2)

            id: wrapper
            width: thumbnailWidth + thumbnailLayout.anchors.margins + thumbnailLayout.spacing
            height: thumbnailHeight + thumbnailLayout.anchors.margins + thumbnailLayout.spacing

            ColumnLayout {
                id: thumbnailLayout
                anchors {
                    fill: parent
                    margins: units.smallSpacing
                }
                spacing: units.largeSpacing

                Rectangle {
                    id: thumbnailItem
                    color: wrapper.ListView.isCurrentItem ? Theme.palette.panel.selectedBackgroundColor : "transparent"
                    radius: units.gridUnit * 0.5
                    width: thumbnailWidth - units.smallSpacing
                    height: thumbnailHeight - units.smallSpacing - label.height

                    SurfaceRenderer {
                        anchors {
                            fill: parent
                            margins: units.smallSpacing
                        }
                        source: modelData.child

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.AllButtons
                            onClicked: listView.currentIndex = index
                        }
                    }
                }

                Label {
                    id: label
                    text: modelData.child.surface.title ? modelData.child.surface.title : qsTr("Untitled")
                    wrapMode: Text.Wrap
                    color: Theme.palette.panel.textColor
                    font.bold: true
                    style: Text.Raised
                    styleColor: Theme.palette.panel.textEffectColor
                    maximumLineCount: 2
                    opacity: wrapper.ListView.isCurrentItem ? 1.0 : 0.6

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }

    Component.onCompleted: {
        // Show with an animtation
        opacity = 1.0;

        var newIndex = compositorRoot.activeWindowIndex + 1;
        if (newIndex === compositorRoot.surfaceModel.count)
            newIndex = 0;
        listView.currentIndex = newIndex;
    }
}
