include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${VibeCore_INCLUDE_DIRS}
    ${GreenIsland_INCLUDE_DIRS}
    ${WaylandInclude_INCLUDE_DIRS}
    ${POLKITQT-1_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    desktopshell.cpp
    shellui.cpp
    waylandintegration.cpp
    notificationsdaemon.cpp
    notificationsimage.cpp
    notificationwindow.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop-extension
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification-daemon
)

qt5_add_resources(RESOURCES desktop-shell.qrc)

qt5_add_dbus_adaptor(SOURCES
    ${CMAKE_SOURCE_DIR}/data/interfaces/org.freedesktop.Notifications.xml
    notificationsdaemon.h NotificationsDaemon)

add_executable(greenisland-desktop-shell ${SOURCES} ${RESOURCES})
qt5_use_modules(greenisland-desktop-shell Quick Widgets AccountsService)
set_target_properties(greenisland-desktop-shell PROPERTIES
    COMPILE_FLAGS "-rdynamic"
    LINK_FLAGS "-L${WaylandClient_LIBDIR} -L${POLKITQT-1_LIB_DIR}"
)
target_link_libraries(greenisland-desktop-shell
    ${VibeCore_LIBRARIES}
    ${GreenIsland_LIBRARIES}
    ${WaylandClient_LIBRARIES}
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
)

install(TARGETS greenisland-desktop-shell DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
