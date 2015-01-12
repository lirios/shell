configure_file(
    hawaii.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii.desktop
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hawaii.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/wayland-sessions)
