import qbs 1.0

StaticLibrary {
    name: "LiriShellDBusInterfaces"

    Depends { name: "cpp" }
    Depends {
        name: "Qt"
        submodules: ["core", "dbus", "gui", "qml", "quick", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }

    files: [
        "screenshoter.cpp",
        "screenshooter.h",
    ]

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
