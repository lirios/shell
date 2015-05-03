set(DIRECTORIES
    hawaii-entertainment.directory
    hawaii-internet.directory
    hawaii-main.directory
    hawaii-system.directory
    hawaii-work.directory
)
install(FILES ${DIRECTORIES}
        DESTINATION ${XDG_DIRECTORY_INSTALL_DIR})

install(FILES hawaii-applications.menu
        DESTINATION ${SYSCONF_INSTALL_DIR}/xdg/menus)
