install(
    FILES
        io.liri.desktop.interface.gschema.xml
        io.liri.desktop.keybindings.gschema.xml
        io.liri.desktop.panel.gschema.xml
        io.liri.desktop.peripherals.gschema.xml
        io.liri.shell.gschema.xml	
        io.liri.system.gschema.xml
    DESTINATION
        "${INSTALL_DATADIR}/glib-2.0/schemas"
)

if(IS_ABSOLUTE "${INSTALL_DATADIR}")
    set(DATADIR "${INSTALL_DATADIR}")
else()
    set(DATADIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_DATADIR}")
endif()
configure_file(
    "io.liri.desktop.background.gschema.xml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.background.gschema.xml"
    @ONLY
)
configure_file(
    "io.liri.desktop.lockscreen.gschema.xml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.lockscreen.gschema.xml"
    @ONLY
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.background.gschema.xml"
        "${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.lockscreen.gschema.xml"
    DESTINATION
        "${INSTALL_DATADIR}/glib-2.0/schemas"
)
