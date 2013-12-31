find_package(Qt5 5.2 REQUIRED COMPONENTS Compositor)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${GreenIsland_INCLUDE_DIRS}
    ${WaylandServer_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

# Create the config.h header file
configure_file(
    "${CMAKE_SOURCE_DIR}/src/client/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY
)

add_definitions(
    -DQT_COMPOSITOR_QUICK
)

set(SOURCES
    main.cpp
    compositor.cpp
    shell.cpp
    keybinding.cpp
    clientwindow.cpp
    popupgrabber.cpp
    shellsurface.cpp
    workspace.cpp
    grab.cpp
    notifications.cpp
    logging.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
qtwayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)
qtwayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)

qt5_add_resources(RESOURCES hawaii-shell.qrc)

add_executable(hawaii-shell ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-shell Quick Compositor)
set_target_properties(hawaii-shell PROPERTIES
    LINK_FLAGS -L${WaylandServer_LIBDIR}
)
target_link_libraries(hawaii-shell
    ${GreenIsland_LIBRARIES}
    ${WaylandServer_LIBRARIES}
)

install(TARGETS hawaii-shell DESTINATION ${CMAKE_INSTALL_BINDIR})
