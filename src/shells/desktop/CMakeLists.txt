find_package(ALSA)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${POLKITQT-1_INCLUDE_DIR}
    ${WaylandClient_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/daemons
    ${CMAKE_CURRENT_SOURCE_DIR}/launcher
    ${CMAKE_CURRENT_SOURCE_DIR}/windowtypes
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/services/powermanager
    ${CMAKE_CURRENT_SOURCE_DIR}/services/volumecontrol
    ${CMAKE_CURRENT_SOURCE_DIR}/services/sessionmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/settings
)

set(SOURCES
    appcategories.cpp
    applicationiconprovider.cpp
    applicationsmodel.cpp
    desktopshell.cpp
    enums.cpp
    keybinding.cpp
    main.cpp
    registration.cpp
    shellui.cpp
    shortcut.cpp
    window.cpp
    workspace.cpp
    daemons/notificationsdaemon.cpp
    daemons/notificationsimage.cpp
    daemons/policykitagent.cpp
    launcher/launcheritem.cpp
    launcher/launchermodel.cpp
    services/servicefactory.cpp
    services/powermanager/powermanagerbackend.cpp
    services/powermanager/powermanager.cpp
    services/powermanager/systemdpowerbackend.cpp
    services/powermanager/upowerpowerbackend.cpp
    services/volumecontrol/volumecontrol.cpp
    services/sessionmanager/sessionmanager.cpp
    settings/backgroundsettings.cpp
    settings/launchersettings.cpp
    windowtypes/backgroundwindow.cpp
    windowtypes/grabwindow.cpp
    windowtypes/launcherwindow.cpp
    windowtypes/lockscreenwindow.cpp
    windowtypes/notificationwindow.cpp
    windowtypes/overlaywindow.cpp
    windowtypes/panelwindow.cpp
    windowtypes/shellwindow.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification
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
    ${ALSA_LIBRARIES}
)

install(TARGETS hawaii-desktop-shell DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
