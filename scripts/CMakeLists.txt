# Generate the starthawaii script
configure_file(starthawaii.in ${CMAKE_CURRENT_BINARY_DIR}/starthawaii @ONLY)
configure_file(hawaii-session.in ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/starthawaii
        DESTINATION ${BIN_INSTALL_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session
        DESTINATION ${BIN_INSTALL_DIR})
