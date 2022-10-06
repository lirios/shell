if(IS_ABSOLUTE "${INSTALL_BINDIR}")
    set(ABSOLUTE_BINDIR "${INSTALL_BINDIR}")
else()
    set(ABSOLUTE_BINDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR}")
endif()
if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(ABSOLUTE_LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(ABSOLUTE_LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()
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
        "${INSTALL_SYSTEMDUSERUNITDIR}"
)
