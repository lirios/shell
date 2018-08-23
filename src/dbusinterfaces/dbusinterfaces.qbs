import qbs 1.0

StaticLibrary {
    readonly property bool screenCastEnabled: {
        if (!project.screenCastEnabled)
            return false;
        return PipeWire01.found || PipeWire02.found;
    }

    name: "LiriShellDBusInterfaces"

    Depends { name: "lirideployment" }
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
        "outputchangeset.cpp",
        "outputchangeset.h",
        "outputconfiguration.cpp",
        "outputconfiguration.h",
        "outputconfigurationadaptor.cpp",
        "outputconfigurationadaptor.h",
        "outputdevice.cpp",
        "outputdevice.h",
        "outputdeviceadaptor.cpp",
        "outputdeviceadaptor.h",
        "outputmanagement.cpp",
        "outputmanagement.h",
        "outputmanagementadaptor.cpp",
        "outputmanagementadaptor.h",
        "outputs.cpp",
        "outputs.h",
        "outputsadaptor.cpp",
        "outputsadaptor.h",
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

    Group {
        name: "D-Bus Interfaces"
        files: [
            "data/io.liri.Shell.OutputConfiguration1.xml",
            "data/io.liri.Shell.OutputDevice1.xml",
            "data/io.liri.Shell.OutputManagement1.xml",
            "data/io.liri.Shell.Outputs.xml",
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
