import qbs 1.0
import qbs.Probes

QtGuiApplication {
    name: "liri-shell"
    targetName: "liri-shell"

    Depends { name: "lirideployment" }
    Depends { name: "GitRevision" }
    Depends {
        name: "Qt"
        submodules: ["core", "core-private", "concurrent", "dbus", "gui", "gui-private", "svg", "qml", "quick", "quickcontrols2", "waylandcompositor"]
        versionAtLeast: project.minimumQtVersion
    }
    Depends { name: "sigwatch" }
    Depends { name: "Qt5GSettings" }
    Depends { name: "Qt5Xdg" }
    Depends { name: "LiriLogind" }
    Depends { name: "LiriLocalDevice" }
    Depends { name: "LiriShellDBusInterfaces" }
    Depends { name: "systemd" }
    Depends { name: "pam" }
    Depends { name: "WaylandScanner" }
    Depends { name: "Wayland.server" }

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

    cpp.commonCompilerFlags: base.concat(["-Wno-deprecated-declarations"])
    cpp.defines: {
        var defines = base.concat([
            'LIRISHELL_VERSION="' + project.version + '"',
            "QT_WAYLAND_COMPOSITOR_QUICK",
        ]);
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
        "main.cpp",
        "application.cpp",
        "application.h",
        "declarative/indicatorsmodel.cpp",
        "declarative/indicatorsmodel.h",
        "declarative/inputsettings.cpp",
        "declarative/inputsettings.h",
        "declarative/outputsettings.cpp",
        "declarative/outputsettings.h",
        "declarative/quickoutput.cpp",
        "declarative/quickoutput.h",
        "declarative/screenmodel.cpp",
        "declarative/screenmodel.h",
        "extensions/gtkshell.cpp",
        "extensions/gtkshell.h",
        "extensions/gtkshell_p.h",
        "extensions/outputchangeset.cpp",
        "extensions/outputchangeset.h",
        "extensions/outputconfiguration.cpp",
        "extensions/outputconfiguration.h",
        "extensions/outputconfiguration_p.h",
        "extensions/outputmanagement.cpp",
        "extensions/outputmanagement.h",
        "extensions/outputmanagement_p.h",
        "extensions/quickoutputconfiguration.cpp",
        "extensions/quickoutputconfiguration.h",
        "multimediakeys/multimediakeys.cpp",
        "multimediakeys/multimediakeys.h",
        "logging.cpp",
        "logging_p.h",
        "onscreendisplay.cpp",
        "onscreendisplay.h",
        "processlauncher/processlauncher.cpp",
        "processlauncher/processlauncher.h",
        "qmlregistration.cpp",
        "qmlregistration.h",
        "sessionmanager/authenticator.cpp",
        "sessionmanager/authenticator.h",
        "sessionmanager/qmlauthenticator.cpp",
        "sessionmanager/qmlauthenticator.h",
        "sessionmanager/sessionmanager.cpp",
        "sessionmanager/sessionmanager.h",
        "sessionmanager/loginmanager/fakebackend.cpp",
        "sessionmanager/loginmanager/fakebackend.h",
        "sessionmanager/loginmanager/logindbackend.cpp",
        "sessionmanager/loginmanager/logindbackend.h",
        "sessionmanager/loginmanager/loginmanager.cpp",
        "sessionmanager/loginmanager/loginmanager.h",
        "sessionmanager/loginmanager/loginmanagerbackend.cpp",
        "sessionmanager/loginmanager/loginmanagerbackend.h",
        "sessionmanager/screensaver/screensaver.cpp",
        "sessionmanager/screensaver/screensaver.h",
    ]

    Group {
        name: "Resource Data"
        prefix: "qml/"
        files: [
            "base/Keyboard.qml",
            "base/OutputConfiguration.qml",
            "components/CloseButton.qml",
            "components/HotCorner.qml",
            "components/Overlay.qml",
            "components/PagedGrid.qml",
            "desktop/Desktop.qml",
            "desktop/DesktopWidgets.qml",
            "desktop/IdleDimmer.qml",
            "desktop/OutputInfo.qml",
            "desktop/PresentEffect.js",
            "desktop/PresentWindowsChrome.qml",
            "desktop/RunCommand.qml",
            "desktop/ScreenView.qml",
            "desktop/ScreenZoom.qml",
            "desktop/Shell.qml",
            "desktop/WindowSwitcher.qml",
            "desktop/Workspace.qml",
            "desktop/WorkspacesView.qml",
            "error/ErrorCompositor.qml",
            "error/ErrorOutput.qml",
            "error/ErrorScreenView.qml",
            "images/disk.svg",
            "images/eject.svg",
            "images/harddisk.svg",
            "images/hibernate.svg",
            "images/lan-connect.svg",
            "images/lan-disconnect.svg",
            "images/logout.svg",
            "images/reload.svg",
            "images/sleep.svg",
            "images/wallpaper.png",
            "indicators/launcher/AppsIcon.qml",
            "indicators/launcher/Dot.qml",
            "indicators/LauncherIndicator.qml",
            "indicators/NotificationsIndicator.qml",
            "indicators/SettingsIndicator.qml",
            "indicators/StorageIndicator.qml",
            "launcher/FrequentAppsView.qml",
            "launcher/LauncherCategories.qml",
            "launcher/LauncherDelegate.qml",
            "launcher/LauncherGridDelegate.qml",
            "launcher/LauncherGridView.qml",
            "launcher/LauncherMenu.qml",
            "launcher/LauncherPopOver.qml",
            "launcher/Launcher.qml",
            "launcher/LauncherShutdownActions.qml",
            "notifications/NotificationDelegate.qml",
            "notifications/NotificationItem.qml",
            "notifications/Notifications.qml",
            "overlays/HelpOverlay.qml",
            "overlays/UnresponsiveOverlay.qml",
            "panel/IndicatorsRow.qml",
            "panel/Panel.qml",
            "screens/AuthDialog.qml",
            "screens/LockScreen.qml",
            "screens/LogoutScreen.qml",
            "screens/PowerDialog.qml",
            "screens/PowerScreen.qml",
            "screens/SplashScreen.qml",
            "windows/ChromeMenu.qml",
            "windows/Decoration.qml",
            "windows/DecorationButton.qml",
            "windows/MoveItem.qml",
            "windows/WaylandChrome.qml",
            "windows/window-close.svg",
            "windows/window-maximize.svg",
            "windows/window-minimize.svg",
            "windows/window-restore.svg",
            "windows/WlShellWindow.qml",
            "windows/XdgPopupV5Window.qml",
            "windows/XdgSurfaceV5Window.qml",
            "windows/XWaylandChrome.qml",
            "Compositor.qml",
            "KeyBindings.qml",
            "Output.qml",
            "ScreenManager.qml",
            "ShellSettings.qml",
            "XWayland.qml",
        ]
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
        name: "Wayland Protocols"
        files: [
            "../../data/protocols/gtk-shell.xml",
            "../../data/protocols/liri-outputmanagement.xml",
        ]
        fileTags: ["wayland.server.protocol"]
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
