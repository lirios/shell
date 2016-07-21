configure_file(hawaii-session.in ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session
        DESTINATION ${BIN_INSTALL_DIR})
