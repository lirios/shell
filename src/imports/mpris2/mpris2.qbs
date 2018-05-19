import qbs 1.0

LiriQmlPlugin {
    name: "mpris2plugin"
    pluginPath: "Liri/Mpris"

    Depends { name: "Qt"; submodules: ["dbus"] }

    Qt.dbus.xml2CppHeaderFlags: "-N"

    files: [
        "mpris2engine.cpp",
        "mpris2engine.h",
        "mpris2player.cpp",
        "mpris2player.h",
        "plugin.cpp",
        "plugins.qmltypes",
        "qmldir",
    ]

    Group {
        name: "D-Bus Interfaces"
        files: [
            "org.freedesktop.DBus.Properties.xml",
            "org.mpris.MediaPlayer2.Player.xml",
            "org.mpris.MediaPlayer2.xml"
        ]
        fileTags: ["qt.dbus.interface"]
    }
}
