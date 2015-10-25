configure_file(
    org.hawaiios.desktop.background.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaiios.desktop.background.gschema.xml
)
configure_file(
    org.hawaiios.desktop.lockscreen.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaiios.desktop.lockscreen.gschema.xml
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaiios.desktop.background.gschema.xml
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaiios.desktop.lockscreen.gschema.xml
    org.hawaiios.desktop.interface.gschema.xml
    org.hawaiios.desktop.peripherals.gschema.xml
    org.hawaiios.desktop.panel.gschema.xml
    org.hawaiios.desktop.keybindings.gschema.xml
    org.hawaiios.desktop.session.gschema.xml
)

install(FILES ${FILES} DESTINATION ${GSETTINGS_SCHEMA_DIR})
