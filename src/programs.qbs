import qbs 1.0

Project {
    name: "Programs"

    references: [
        "compositor/compositor.qbs",
        "dbusinterfaces/dbusinterfaces.qbs",
        "helper/helper.qbs",
        "scripts/scripts.qbs",
    ]
}
