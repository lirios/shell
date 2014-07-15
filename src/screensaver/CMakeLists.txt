include_directories(
    ${WaylandClient_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    screensavercontroller.cpp
    screensaverview.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)

qt5_add_resources(RESOURCES screensaver.qrc)

add_executable(hawaii-screensaver ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-screensaver Quick)
set_target_properties(hawaii-screensaver PROPERTIES
    LINK_FLAGS "-L${WaylandClient_LIBDIR}"
)
target_link_libraries(hawaii-screensaver ${WaylandClient_LIBRARIES})

install(TARGETS hawaii-screensaver DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
