include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${VibeCore_INCLUDE_DIRS}
    ${GreenIsland_INCLUDE_DIRS}
    ${WaylandInclude_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    desktopshell.cpp
    shellwindow.cpp
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

add_executable(hawaii-desktop-shell ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-desktop-shell Quick Widgets AccountsService)
set_target_properties(hawaii-desktop-shell PROPERTIES
    COMPILE_FLAGS "-rdynamic"
    LINK_FLAGS "-L${WaylandClient_LIBDIR}"
)
target_link_libraries(hawaii-desktop-shell
    ${VibeCore_LIBRARIES}
    ${GreenIsland_LIBRARIES}
    ${WaylandClient_LIBRARIES}
)

install(TARGETS hawaii-desktop-shell DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
