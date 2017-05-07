import qbs 1.0

LiriDynamicLibrary {
    name: "Liri.LoginManager"
    targetName: "loginmanagerplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["qml", "quick"] }
    Depends { name: "Qt5AccountsService" }

    cpp.defines: []

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
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Liri/LoginManager"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
