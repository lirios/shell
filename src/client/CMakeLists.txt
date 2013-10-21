find_package(ALSA)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${POLKITQT-1_INCLUDE_DIR}
    ${WaylandClient_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/libqtxdg
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/daemons
    ${CMAKE_CURRENT_SOURCE_DIR}/launcher
    ${CMAKE_CURRENT_SOURCE_DIR}/windowtypes
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/services/policykit-agent
    ${CMAKE_CURRENT_SOURCE_DIR}/services/powermanager
    ${CMAKE_CURRENT_SOURCE_DIR}/services/volumecontrol
    ${CMAKE_CURRENT_SOURCE_DIR}/services/sessionmanager
)

# Create the config.h header file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/libqtxdg/xdgdesktopfile.cpp
    appcategories.cpp
    appinfo.cpp
    applicationiconprovider.cpp
    applicationsmodel.cpp
    element.cpp
    elementfactory.cpp
    enums.cpp
    keybinding.cpp
    hawaiishell.cpp
    main.cpp
    registration.cpp
    shellui.cpp
    shellscreen.cpp
    shortcut.cpp
    window.cpp
    workspace.cpp
    daemons/notificationimageprovider.cpp
    daemons/notificationsdaemon.cpp
    daemons/notificationsimage.cpp
    launcher/launcheritem.cpp
    launcher/launchermodel.cpp
    services/servicefactory.cpp
    services/policykit-agent/policykitagent.cpp
    services/powermanager/powermanagerbackend.cpp
    services/powermanager/powermanager.cpp
    services/powermanager/systemdpowerbackend.cpp
    services/powermanager/upowerpowerbackend.cpp
    services/volumecontrol/volumecontrol.cpp
    services/sessionmanager/sessionmanager.cpp
    windowtypes/backgroundwindow.cpp
    windowtypes/dialogwindow.cpp
    windowtypes/grabwindow.cpp
    windowtypes/launcherwindow.cpp
    windowtypes/lockscreenwindow.cpp
    windowtypes/notificationwindow.cpp
    windowtypes/overlaywindow.cpp
    windowtypes/panelwindow.cpp
    windowtypes/popupwindow.cpp
    windowtypes/shellwindow.cpp
    windowtypes/tooltipwindow.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
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
qt5_use_modules(hawaii-desktop-shell Quick Widgets AccountsService)
set_target_properties(hawaii-desktop-shell PROPERTIES
    COMPILE_FLAGS "-rdynamic"
    LINK_FLAGS "-L${WaylandClient_LIBDIR} -L${POLKITQT-1_LIB_DIR}"
)
target_link_libraries(hawaii-desktop-shell
    ${WaylandClient_LIBRARIES}
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${ALSA_LIBRARIES}
    HawaiiCore
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii-desktop-shell)

install(TARGETS hawaii-desktop-shell DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
