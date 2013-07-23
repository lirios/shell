include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${GreenIsland_INCLUDE_DIRS}
    ${WaylandServer_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/headers
)

add_definitions(
    -DQT_PLUGIN
    -DQT_COMPOSITOR_QUICK
)

set(SOURCES
    main.cpp
    compositor.cpp
    desktopshellserver.cpp
)

wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop-shell
)
wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop-shell
)

qt5_add_resources(RESOURCES desktop.qrc)

add_library(desktop SHARED ${SOURCES} ${RESOURCES})
qt5_use_modules(desktop Quick Compositor)
set_target_properties(desktop PROPERTIES
    PREFIX ""
    LINK_FLAGS -L${WaylandServer_LIBDIR}
)
target_link_libraries(desktop
    ${GreenIsland_LIBRARIES}
    ${WaylandServer_LIBRARIES}
)

install(TARGETS desktop
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/plugins/greenisland/compositors)
