import qbs 1.0

Project {
    name: "Data"

    references: [
        "menu/menu.qbs",
        "settings/settings.qbs",
        "systemd/systemd.qbs",
        "wayland-sessions/wayland-sessions.qbs",
    ]
}
