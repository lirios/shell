find_package(Qt5 5.2 REQUIRED COMPONENTS Compositor)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${GreenIsland_INCLUDE_DIRS}
    ${WaylandServer_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

add_definitions(
    -DQT_COMPOSITOR_QUICK
)

set(SOURCES
    clientwindow.cpp
    compositor.cpp
    keybinding.cpp
    logging.cpp
    main.cpp
    notifications.cpp
    panelmanager.cpp
    popupgrabber.cpp
    screensaver.cpp
    shell.cpp
    shellpanelsurface.cpp
    shellpopupsurface.cpp
    shellsurface.cpp
    workspace.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii hawaii_
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii hawaii_
)
qtwayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii hawaii_
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)
qtwayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
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

set(LINK_FLAGS
    -L${WaylandServer_LIBDIR}
)
set(LIBRARIES
    ${GreenIsland_LIBRARIES}
    ${WaylandServer_LIBRARIES}
)
if(systemd_FOUND)
    set(LINK_FLAGS "${LINK_FLAGS} -L${systemd_LIBDIR}")
    set(LIBRARIES ${LIBRARIES} ${systemd_LIBRARIES})
endif()

add_executable(hawaii-shell ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-shell Quick Compositor)
set_target_properties(hawaii-shell PROPERTIES LINK_FLAGS ${LINK_FLAGS})
target_link_libraries(hawaii-shell ${LIBRARIES})

install(TARGETS hawaii-shell DESTINATION ${CMAKE_INSTALL_BINDIR})
