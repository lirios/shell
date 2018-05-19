import qbs 1.0

LiriQmlPlugin {
    name: "polkitplugin"
    pluginPath: "Liri/PolicyKit"

    Depends { name: "Qt"; submodules: ["core", "dbus"] }
    Depends { name: "polkit_qt5"; submodules: ["core", "agent"] }

    condition: {
        if (!polkit_qt5.core.found)
            throw "polkit-qt5-core-1 is required to build " + targetName;

        if (!polkit_qt5.agent.found)
            throw "polkit-qt5-agent-1 is required to build " + targetName;

        return true;
    }

    files: [
        "listener_p.cpp",
        "listener_p.h",
        "plugin.cpp",
        "plugins.qmltypes",
        "policykitagent.cpp",
        "policykitagent.h",
        "qmldir",
    ]
}
