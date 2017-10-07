import qbs 1.0
import qbs.Probes

QtGuiApplication {
    name: "liri-shell"
    targetName: "liri-shell"

    Depends { name: "lirideployment" }
    Depends { name: "GitRevision" }
    Depends {
        name: "Qt"
        submodules: ["core", "dbus", "gui", "widgets", "quickcontrols2", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }
    Depends { name: "sigwatch" }
    Depends { name: "Qt5GSettings" }
    Depends { name: "Qt5Xdg" }
    Depends { name: "systemd" }
    Depends { name: "LiriWaylandServer" }
    Depends { name: "pam" }

    LiriIncludeProbe {
        id: haveSysPrctl
        names: ["sys/prctl.h"]
    }

    condition: {
        if (!Qt5Xdg.found) {
            console.error("Qt5Xdg is required to build " + targetName);
            return false;
        }

        if (!pam.found) {
            console.error("PAM is required to build " + targetName);
            return false;
        }

        return true;
    }

    cpp.defines: {
        var defines = base.concat(['LIRISHELL_VERSION="' + project.version + '"']);
        if (project.developmentBuild)
            defines.push("DEVELOPMENT_BUILD");
        if (haveSysPrctl.found)
            defines.push("HAVE_SYS_PRCTL_H");
        if (systemd.found)
            defines.push("HAVE_SYSTEMD");
        return defines;
    }
    cpp.includePaths: base.concat([
        product.sourceDirectory
    ])

    GitRevision.sourceDirectory: product.sourceDirectory + "/../.."

    Qt.core.resourcePrefix: "/"
    Qt.core.resourceSourceBase: sourceDirectory

    files: [
        "*.cpp",
        "*.h",
        "processlauncher/*.cpp",
        "processlauncher/*.h",
        "sessionmanager/*.cpp",
        "sessionmanager/*.h",
        "sessionmanager/loginmanager/*.cpp",
        "sessionmanager/loginmanager/*.h",
        "sessionmanager/powermanager/*.cpp",
        "sessionmanager/powermanager/*.h",
        "sessionmanager/screensaver/*.cpp",
        "sessionmanager/screensaver/*.h",
    ]

    Group {
        name: "Resource Data"
        prefix: "qml/"
        files: ["**"]
        fileTags: ["qt.core.resource_data"]
    }

    Group {
        name: "D-Bus Adaptors"
        files: [
            "processlauncher/io.liri.ProcessLauncher.xml",
            "sessionmanager/screensaver/org.freedesktop.ScreenSaver.xml"
        ]
        fileTags: ["qt.dbus.adaptor"]
    }

    Group {
        name: "Git Revision"
        files: ["../../headers/gitsha1.h.in"]
        fileTags: ["liri.gitsha"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.binDir
        fileTagsFilter: product.type
    }
}
