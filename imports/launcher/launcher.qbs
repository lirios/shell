import qbs 1.0

LiriDynamicLibrary {
    name: "Liri.Launcher"
    targetName: "launcherplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["dbus", "xml", "qml", "quick", "sql", "waylandcompositor"] }
    Depends { name: "LiriCore" }
    Depends { name: "Qt5GSettings" }
    Depends { name: "Qt5Xdg" }

    condition: {
        if (!Qt5Xdg.found) {
            console.error("Qt5Xdg is required to build " + targetName);
            return false;
        }

        return true;
    }

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
        qbs.installDir: lirideployment.qmlDir + "/Liri/Launcher"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
