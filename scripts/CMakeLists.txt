configure_file(starthawaii.in ${CMAKE_CURRENT_BINARY_DIR}/starthawaii @ONLY)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/starthawaii
        DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

configure_file(hawaii.in ${CMAKE_CURRENT_BINARY_DIR}/hawaii @ONLY)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/hawaii
        DESTINATION ${CMAKE_INSTALL_BINDIR})
