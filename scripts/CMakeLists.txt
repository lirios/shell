configure_file(liri-session.in ${CMAKE_CURRENT_BINARY_DIR}/liri-session @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/liri-session
        DESTINATION ${BIN_INSTALL_DIR})
