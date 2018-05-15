import qbs 1.0

LiriQmlPlugin {
    name: "loginmanagerplugin"
    pluginPath: "Liri/LoginManager"

    Depends { name: "Qt5AccountsService" }

    cpp.defines: []

    files: [
        "plugin.cpp",
        "plugins.qmltypes",
        "qmldir",
        "usersmodel.cpp",
        "usersmodel.h",
    ]
}
