# Generate the starthawaii script
configure_file(starthawaii.in ${CMAKE_CURRENT_BINARY_DIR}/starthawaii @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/starthawaii
        DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
