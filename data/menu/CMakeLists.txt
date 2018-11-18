install(
    FILES
        liri-entertainment.directory
        liri-internet.directory
        liri-main.directory
        liri-system.directory
        liri-work.directory
    DESTINATION
        "${INSTALL_DATADIR}/desktop-directories"
)

install(
    FILES
        liri-applications.menu
    DESTINATION
        "${INSTALL_SYSCONFDIR}/xdg/menus"
)
