# Find ALSA
find_package(ALSA)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${QtConfiguration_INCLUDE_DIRS}
    ${HawaiiShell_INCLUDE_DIRS}
    ${WaylandClient_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/views
    ${CMAKE_CURRENT_SOURCE_DIR}/windowtypes
    ${CMAKE_CURRENT_SOURCE_DIR}/scripting
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/services/datetime
    ${CMAKE_CURRENT_SOURCE_DIR}/services/powermanager
    ${CMAKE_CURRENT_SOURCE_DIR}/services/processlauncher
    ${CMAKE_CURRENT_SOURCE_DIR}/services/volumecontrol
    ${CMAKE_CURRENT_SOURCE_DIR}/services/sessionmanager
    ${CMAKE_CURRENT_SOURCE_DIR}/wayland
)

set(SOURCES
    enums.cpp
    keybinding.cpp
    main.cpp
    registration.cpp
    registrylistener.cpp
    shellclient.cpp
    shellcontroller.cpp
    shellmanager.cpp
    shellsurfaceclient.cpp
    shellui.cpp
    shortcut.cpp
    window.cpp
    workspace.cpp
    scripting/appinterface.cpp
    scripting/panelinterface.cpp
    scripting/scriptengine.cpp
    services/servicefactory.cpp
    services/datetime/datetime.cpp
    services/powermanager/powermanagerbackend.cpp
    services/powermanager/powermanager.cpp
    services/powermanager/systemdpowerbackend.cpp
    services/powermanager/upowerpowerbackend.cpp
    services/processlauncher/processlauncher.cpp
    services/volumecontrol/volumecontrol.cpp
    services/sessionmanager/sessionmanager.cpp
    views/backgroundview.cpp
    views/configview.cpp
    views/desktopview.cpp
    views/lockscreenview.cpp
    views/panelconfigview.cpp
    views/panelview.cpp
    windowtypes/grabwindow.cpp
    wayland/panelsurface.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)

qt5_add_resources(RESOURCES client.qrc)

add_executable(hawaii-shell-client ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-shell-client DBus Quick Widgets Script)
set_target_properties(hawaii-shell-client PROPERTIES
    LINK_FLAGS "-L${WaylandClient_LIBDIR}"
)
target_link_libraries(hawaii-shell-client
    ${WaylandClient_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${HawaiiShell_LIBRARIES}
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii-shell-client)

install(TARGETS hawaii-shell-client DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
