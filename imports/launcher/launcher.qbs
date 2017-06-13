import qbs 1.0

LiriQmlPlugin {
    name: "launcherplugin"
    pluginPath: "Liri/Launcher"

    Depends { name: "Qt"; submodules: ["dbus", "xml", "sql", "waylandcompositor"] }
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

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]
}
