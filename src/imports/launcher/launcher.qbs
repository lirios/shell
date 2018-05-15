import qbs 1.0

LiriQmlPlugin {
    name: "launcherplugin"
    pluginPath: "Liri/Launcher"

    Depends {
        name: "Qt"
        submodules: ["dbus", "xml", "sql", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }
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

    files: [
        "application.cpp",
        "application.h",
        "applicationmanager.cpp",
        "applicationmanager.h",
        "appusage.cpp",
        "appusage.h",
        "categoriesmodel.cpp",
        "categoriesmodel.h",
        "frequentmodel.cpp",
        "frequentmodel.h",
        "launchermodel.cpp",
        "launchermodel.h",
        "pagemodel.cpp",
        "pagemodel.h",
        "plugin.cpp",
        "plugins.qmltypes",
        "processrunner.cpp",
        "processrunner.h",
        "qmldir",
        "usagetracker.cpp",
        "usagetracker.h",
        "utils.cpp",
        "utils.h",
    ]
}
