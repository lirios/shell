import qbs 1.0

LiriQmlPlugin {
    name: "loginmanagerplugin"
    pluginPath: "Liri/LoginManager"

    Depends { name: "Qt5AccountsService" }

    cpp.defines: []

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]
}
