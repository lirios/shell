import qbs 1.0

Project {
    name: "Shell"

    readonly property string version: "0.9.0"

    readonly property string minimumQtVersion: "5.9.0"

    property bool useStaticAnalyzer: false

    property bool developmentBuild: false
    property string systemdUserUnitDir: "lib/systemd/user"

    condition: qbs.targetOS.contains("linux") && !qbs.targetOS.contains("android")

    minimumQbsVersion: "1.8.0"

    references: [
        "3rdparty/3rdparty.qbs",
        "data/data.qbs",
        "src/imports/imports.qbs",
        "src/programs.qbs",
    ]
}
