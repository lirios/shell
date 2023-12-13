set(ABSOLUTE_BINDIR "${KDE_INSTALL_FULL_BINDIR}")
set(ABSOLUTE_LIBEXECDIR "${KDE_INSTALL_FULL_LIBEXECDIR}")
configure_file(
    "liri-shell.service.in"
    "${CMAKE_CURRENT_BINARY_DIR}/liri-shell.service"
    @ONLY
)

install(
    FILES
        liri-shell.target
        "${CMAKE_CURRENT_BINARY_DIR}/liri-shell.service"
    DESTINATION
        "${KDE_INSTALL_SYSTEMDUSERUNITDIR}"
)
