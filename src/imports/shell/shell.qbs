import qbs 1.0
import qbs.FileInfo

LiriQmlPlugin {
    name: "shellplugin"
    pluginPath: "Liri/Shell"

    Depends { name: "Qt"; submodules: ["core", "core-private", "waylandcompositor-private"] }
    Depends { name: "WaylandScanner" }
    Depends { name: "LiriWaylandServer" }

    cpp.defines: ['INSTALL_LIBEXECDIR="' + FileInfo.joinPaths(qbs.installRoot, qbs.installPrefix, lirideployment.libexecDir) + '"']
    cpp.includePaths: base.concat([product.buildDirectory])

    files: ["*.cpp", "*.h", "qmldir", "*.qml", "*.qmltypes"]

    Group {
        name: "Wayland Protocols"
        files: [
            "../../../data/protocols/shell-helper.xml"
        ]
        fileTags: ["wayland.server.protocol"]
    }
}
