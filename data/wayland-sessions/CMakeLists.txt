configure_file(
    liri.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/liri.desktop
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liri.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/wayland-sessions)
