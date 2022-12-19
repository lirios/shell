import QtQuick 2.0

ListModel {
    id: userModel

    property int lastIndex: 1

    ListElement {
        name: "plfiorini"
        realName: "Pier Luigi Fiorini"
        icon: "/usr/share/pixmaps/faces/grapes.jpg"
        homeDir: "/home/plfiorini"
    }
    ListElement {
        name: "ned"
        realName: "Eddard Stark"
        icon: "/home/ned/.face.icon"
        homeDir: "/home/ned"
    }
    ListElement {
        name: "kinginthenorth"
        realName: "Robb Stark"
        icon: ""
        homeDir: ""
    }
    ListElement {
        name: "jon"
        realName: "Jon Snow"
        icon: ""
        homeDir:""
    }
    ListElement {
        name: "imp"
        realName: "Tyrion Lannister"
        icon: ""
        homeDir: ""
    }

    Component.onCompleted: {
        for (var i = 0; i < userModel.count; i++) {
            if (userModel.get(i).name == "imp") {
                userModel.get(i).icon = Qt.resolvedUrl("tyrion.png")
                break
            }
        }
    }
}
