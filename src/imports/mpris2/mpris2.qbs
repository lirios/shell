import qbs 1.0

LiriQmlPlugin {
    name: "mpris2plugin"
    pluginPath: "Vibe/Mpris"

    Depends { name: "Qt"; submodules: ["dbus"] }

    Qt.dbus.xml2CppHeaderFlags: "-N"

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]

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
