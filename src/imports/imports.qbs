import qbs 1.0

Project {
    name: "QML Plugins"

    references: [
        "launcher/launcher.qbs",
        "loginmanager/loginmanager.qbs",
        "mpris2/mpris2.qbs",
        "policykit/policykit.qbs",
        "shell/shell.qbs",
    ]
}
