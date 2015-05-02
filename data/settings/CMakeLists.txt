configure_file(
    org.hawaii.desktop.background.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaii.desktop.background.gschema.xml
)
configure_file(
    org.hawaii.desktop.lockscreen.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaii.desktop.lockscreen.gschema.xml
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaii.desktop.background.gschema.xml
    ${CMAKE_CURRENT_BINARY_DIR}/org.hawaii.desktop.lockscreen.gschema.xml
    org.hawaii.desktop.interface.gschema.xml
    org.hawaii.desktop.peripherals.gschema.xml
)

install(FILES ${FILES} DESTINATION ${GSETTINGS_SCHEMA_DIR})
