import qbs 1.0

LiriDynamicLibrary {
    name: "Liri.Shell"
    targetName: "shellplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "core-private", "qml", "quick", "waylandcompositor-private"] }
    Depends { name: "WaylandScanner" }
    Depends { name: "LiriWaylandServer" }

    cpp.defines: ['INSTALL_LIBEXECDIR="' + qbs.installRoot + "/" + lirideployment.libexecDir + '"']
    cpp.includePaths: base.concat([product.buildDirectory])

    files: ["*.cpp", "*.h"]

    Group {
        name: "QML Files"
        files: [
            "*.qml",
            "qmldir",
            "plugins.qmltypes"
        ]
        fileTags: ["qml"]
    }

    Group {
        name: "Wayland Protocols"
        files: [
            "../../data/protocols/shell-helper.xml"
        ]
        fileTags: ["wayland.server.protocol"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Liri/Shell"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
