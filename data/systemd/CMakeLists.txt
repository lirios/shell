if(NOT IS_ABSOLUTE "${INSTALL_BINDIR}")
    set(INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR}")
endif()
configure_file(
    "liri.service.in"
    "${CMAKE_CURRENT_BINARY_DIR}/liri.service"
    @ONLY
)

install(
    FILES
        liri.target
        "${CMAKE_CURRENT_BINARY_DIR}/liri.service"
    DESTINATION
        "${INSTALL_SYSTEMDUSERUNITDIR}"
)
