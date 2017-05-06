import qbs 1.0

LiriDynamicLibrary {
    name: "Vibe.Mpris"
    targetName: "mpris2plugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["dbus", "qml", "quick"] }

    Qt.dbus.xml2CppHeaderFlags: "-N"

    files: ["*.cpp", "*.h"]

    Group {
        name: "QML Files"
        files: [
            "qmldir",
            "plugins.qmltypes"
        ]
        fileTags: ["qml"]
    }

    Group {
        name: "D-Bus Interfaces"
        files: [
            "org.freedesktop.DBus.Properties.xml",
            "org.mpris.MediaPlayer2.Player.xml",
            "org.mpris.MediaPlayer2.xml"
        ]
        fileTags: ["qt.dbus.interface"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Vibe/Mpris"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
