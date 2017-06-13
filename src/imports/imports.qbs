import qbs 1.0

Project {
    name: "QML Plugins"

    references: [
        "launcher/launcher.qbs",
        "loginmanager/loginmanager.qbs",
        "shell/shell.qbs",
    ]
}
