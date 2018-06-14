import qbs 1.0
import qbs.FileInfo

LiriQmlPlugin {
    name: "shellplugin"
    pluginPath: "Liri/Shell"

    Depends {
        name: "Qt"
        submodules: ["core", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }
    Depends { name: "WaylandScanner" }

    cpp.defines: [
        'INSTALL_ROOTDIR="' + qbs.installRoot + '"',
        'INSTALL_LIBEXECDIR="' + FileInfo.joinPaths(qbs.installPrefix, lirideployment.libexecDir) + '"',
    ]
    cpp.includePaths: base.concat([product.buildDirectory])

    files: [
        "Background.qml",
        "DateTime.qml",
        "DateTimeIndicator.qml",
        "Indicator.qml",
        "LoginGreeter.qml",
        "PanelItem.qml",
        "UserDelegate.qml",
        "chromeitem.cpp",
        "chromeitem.h",
        "hotspot.cpp",
        "hotspot.h",
        "keyeventfilter.cpp",
        "keyeventfilter.h",
        "logging_p.cpp",
        "logging_p.h",
        "plugin.cpp",
        "plugins.qmltypes",
        "qmldir",
        "shellhelper.cpp",
        "shellhelper.h",
        "shellhelper_p.h",
        "windowmousetracker.cpp",
        "windowmousetracker.h",
    ]

    Group {
        name: "Wayland Protocols"
        files: [
            "../../../data/protocols/shell-helper.xml"
        ]
        fileTags: ["wayland.server.protocol"]
    }
}
