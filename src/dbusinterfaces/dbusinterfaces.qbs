import qbs 1.0

StaticLibrary {
    readonly property bool screenCastEnabled: {
        if (!project.screenCastEnabled)
            return false;
        return PipeWire01.found || PipeWire02.found;
    }

    name: "LiriShellDBusInterfaces"

    Depends { name: "cpp" }
    Depends {
        name: "Qt"
        submodules: ["core", "dbus", "gui", "qml", "quick", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }
    Depends { name: "LiriPlatformHeaders" }
    Depends {
        condition: project.screenCastEnabled
        name: "PipeWire01"
    }
    Depends {
        condition: project.screenCastEnabled && !PipeWire01.found
        name: "PipeWire02"
    }

    // FIXME: Set language version globally because it seems to be
    // ignored in the group
    cpp.cxxLanguageVersion: "gnu++11"

    cpp.defines: {
        var defines = [];
        if (product.screenCastEnabled)
            defines.push("SCREENCAST_ENABLED");
        return base.concat(defines);
    }

    files: [
        "screencast.cpp",
        "screencast.h",
        "screencastadaptor.cpp",
        "screencastadaptor.h",
        "screenshoter.cpp",
        "screenshooter.h",
    ]

    Group {
        name: "ScreenCast"
        condition: product.screenCastEnabled

        cpp.cxxLanguageVersion: "gnu++11"

        files: [
            "screencaststream.cpp",
            "screencaststream.h",
        ]
    }

    Export {
        Depends { name: "cpp" }
        Depends {
            name: "Qt"
            submodules: ["core", "dbus", "gui", "qml", "quick", "waylandcompositor"]
            versionAtLeast: project.minimumQtVersion
        }

        cpp.includePaths: product.sourceDirectory
    }
}
