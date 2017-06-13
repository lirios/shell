import qbs 1.0

Project {
    name: "Shell"

    readonly property string version: "0.9.0"

    property bool developmentBuild: false
    property string systemdUserUnitDir: "lib/systemd/user"

    condition: qbs.targetOS.contains("linux")

    minimumQbsVersion: "1.8.0"

    qbsSearchPaths: ["qbs/shared"]

    references: [
        "3rdparty/3rdparty.qbs",
        "data/data.qbs",
        "src/imports/imports.qbs",
        "src/programs.qbs",
    ]
}
