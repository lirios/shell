if(NOT IS_ABSOLUTE "${INSTALL_BINDIR}")
    set(INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR}")
endif()
configure_file(
    "liri.desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/liri.desktop"
    @ONLY
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/liri.desktop"
    DESTINATION
        "${INSTALL_DATADIR}/wayland-sessions"
)
