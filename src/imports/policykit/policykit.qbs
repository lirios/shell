import qbs 1.0

LiriDynamicLibrary {
    name: "Vibe.PolicyKit"
    targetName: "polkitplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["core", "core-private", "dbus", "qml", "quick"] }
    Depends { name: "polkit_qt5"; submodules: ["core", "agent"] }

    condition: {
        if (!polkit_qt5.core.found) {
            console.error("polkit-qt5-core-1 is required to build " + targetName);
            return false;
        }

        if (!polkit_qt5.agent.found) {
            console.error("polkit-qt5-agent-1 is required to build " + targetName);
            return false;
        }

        return true;
    }

    cpp.defines: base.concat(['VIBE_VERSION="' + project.version + '"'])

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
        qbs.installDir: lirideployment.qmlDir + "/Vibe/PolicyKit"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
