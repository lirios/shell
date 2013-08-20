include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${POLKITQT-1_INCLUDE_DIR}
    ${WaylandClient_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    appcategories.cpp
    applicationiconprovider.cpp
    applicationsmodel.cpp
    backgroundsettings.cpp
    backgroundwindow.cpp
    declarativeplugin.cpp
    desktopshell.cpp
    enums.cpp
    grabwindow.cpp
    launcheritem.cpp
    launchermodel.cpp
    launchersettings.cpp
    launcherwindow.cpp
    lockscreenwindow.cpp
    main.cpp
    notificationsdaemon.cpp
    notificationsimage.cpp
    notificationwindow.cpp
    panelwindow.cpp
    policykitagent.cpp
    powermanagerbackend.cpp
    powermanager.cpp
    sessionmanager.cpp
    shellui.cpp
    shellwindow.cpp
    systemdpowerbackend.cpp
    upowerpowerbackend.cpp
    waylandintegration.cpp
    window.cpp
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
qt5_use_modules(hawaii-desktop-shell Quick Widgets Configuration AccountsService Xdg)
set_target_properties(hawaii-desktop-shell PROPERTIES
    COMPILE_FLAGS "-rdynamic"
    LINK_FLAGS "-L${WaylandClient_LIBDIR} -L${POLKITQT-1_LIB_DIR}"
)
target_link_libraries(hawaii-desktop-shell
    ${WaylandClient_LIBRARIES}
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
)

install(TARGETS hawaii-desktop-shell DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
