# Find QtAccountsService
find_package(QtAccountsService REQUIRED)

# Find ALSA
find_package(ALSA)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${POLKITQT-1_INCLUDE_DIR}
    ${WaylandClient_INCLUDE_DIRS}
    ${QtAccountsService_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/libqtxdg
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/applications
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/settings
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/includes
    ${CMAKE_BINARY_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/views
    ${CMAKE_CURRENT_SOURCE_DIR}/windowtypes
    ${CMAKE_CURRENT_SOURCE_DIR}/scripting
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/services/datetime
    ${CMAKE_CURRENT_SOURCE_DIR}/services/policykit-agent
    ${CMAKE_CURRENT_SOURCE_DIR}/services/powermanager
    ${CMAKE_CURRENT_SOURCE_DIR}/services/processlauncher
    ${CMAKE_CURRENT_SOURCE_DIR}/services/volumecontrol
    ${CMAKE_CURRENT_SOURCE_DIR}/services/sessionmanager
)

set(SOURCES
    applicationiconprovider.cpp
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
    extensions/elementfactory.cpp
    extensions/elementinfo.cpp
    scripting/appinterface.cpp
    scripting/panelinterface.cpp
    scripting/scriptengine.cpp
    services/servicefactory.cpp
    services/datetime/datetime.cpp
    services/policykit-agent/policykitagent.cpp
    services/powermanager/powermanagerbackend.cpp
    services/powermanager/powermanager.cpp
    services/powermanager/systemdpowerbackend.cpp
    services/powermanager/upowerpowerbackend.cpp
    services/processlauncher/processlauncher.cpp
    services/volumecontrol/volumecontrol.cpp
    services/sessionmanager/sessionmanager.cpp
    views/desktopview.cpp
    views/lockscreenview.cpp
    views/panelview.cpp
    windowtypes/grabwindow.cpp
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
qt5_use_modules(hawaii-shell-client Quick Widgets Script)
set_target_properties(hawaii-shell-client PROPERTIES
    COMPILE_FLAGS "-rdynamic"
    LINK_FLAGS "-L${WaylandClient_LIBDIR} -L${POLKITQT-1_LIB_DIR}"
)
target_link_libraries(hawaii-shell-client
    ${WaylandClient_LIBRARIES}
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${ALSA_LIBRARIES}
    ${QtAccountsService_LIBRARIES}
    HF1HawaiiShell
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii-shell-client)

install(TARGETS hawaii-shell-client DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
