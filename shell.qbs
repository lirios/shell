import qbs 1.0

Project {
    name: "Liri Shell"

    readonly property string version: "0.9.0"

    condition: qbs.targetOS.contains("linux")

    minimumQbsVersion: "1.6"

    qbsSearchPaths: ["qbs/shared", "qbs/local"]

    references: [
        "3rdparty/sigwatch/sigwatch.qbs",
        "compositor/compositor.qbs",
        "data/pam/pam.qbs",
        "data/systemd/systemd.qbs",
        "data/wayland-sessions/wayland-sessions.qbs",
        "helper/helper.qbs",
        "imports/launcher/launcher.qbs",
        "imports/loginmanager/loginmanager.qbs",
        "imports/shell/shell.qbs",
        "scripts/scripts.qbs",
    ]
}
