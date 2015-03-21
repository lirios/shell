configure_file(shellrc.in ${CMAKE_CURRENT_BINARY_DIR}/shellrc)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/shellrc
)

install(FILES ${FILES}
    DESTINATION ${CONFIG_INSTALL_DIR}/hawaii)
