include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${WaylandClient_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    notificationimageprovider.cpp
    notificationsdaemon.cpp
    notificationsimage.cpp
    notificationwindow.cpp
    registrylistener.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)

qt5_add_resources(RESOURCES notifications.qrc)

qt5_add_dbus_adaptor(SOURCES
    org.freedesktop.Notifications.xml
    notificationsdaemon.h NotificationsDaemon)

add_executable(hawaii-notifications-daemon ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-notifications-daemon DBus Quick)
set_target_properties(hawaii-notifications-daemon PROPERTIES
    LINK_FLAGS "-L${WaylandClient_LIBDIR}"
)
target_link_libraries(hawaii-notifications-daemon ${WaylandClient_LIBRARIES})

install(TARGETS hawaii-notifications-daemon DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
