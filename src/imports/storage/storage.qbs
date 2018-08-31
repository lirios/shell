import qbs 1.0

LiriQmlPlugin {
    name: "storageplugin"
    pluginPath: "Liri/Storage"

    Depends { name: "KF5.Solid" }

    files: [
        "plugin.cpp",
        "plugins.qmltypes",
        "qmldir",
        "storagedevice.cpp",
        "storagedevice.h",
        "storagemodel.cpp",
        "storagemodel.h",
    ]
}
