include_directories(
    ${WaylandServer_INCLUDE_DIRS}
    ${Pixman_INCLUDE_DIRS}
    ${Weston_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    ${CMAKE_CURRENT_SOURCE_DIR}/wl_hawaii
    ${CMAKE_CURRENT_SOURCE_DIR}/wl_shell
    ${CMAKE_BINARY_DIR}/headers
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

set(SOURCES
    animation.cpp
    binding.cpp
    effect.cpp
    inputpanel.cpp
    interface.cpp
    layer.cpp
    sessionmanager.cpp
    settings.cpp
    settingsinterface.cpp
    shell.cpp
    shellseat.cpp
    shellsurface.cpp
    transform.cpp
    workspace.cpp
    xwlshell.cpp
    effects/fademovingeffect.cpp
    effects/griddesktops.cpp
    effects/inoutsurfaceeffect.cpp
    effects/minimizeeffect.cpp
    effects/scaleeffect.cpp
    effects/zoomeffect.cpp
    wl_hawaii/desktop-shell.cpp
    wl_hawaii/hawaiiclientwindow.cpp
    wl_hawaii/hawaiiworkspace.cpp
    wl_hawaii/notifications.cpp
    wl_hawaii/panelmanager.cpp
    wl_hawaii/panelsurface.cpp
    wl_hawaii/screensaver.cpp
    wl_hawaii/shellwindow.cpp
    wl_shell/wlshell.cpp
    wl_shell/wlshellsurface.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/settings.xml
    settings
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/settings.xml
    settings
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/input-method.xml
    input-method
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/input-method.xml
    input-method
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/screensaver.xml
    screensaver
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification-daemon
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification-daemon
)

add_library(hawaii-desktop SHARED ${SOURCES})
set_target_properties(hawaii-desktop PROPERTIES PREFIX "")
if(systemd_FOUND)
    set_target_properties(hawaii-desktop PROPERTIES LINK_FLAGS "-L${systemd_LIBDIR}")
    target_link_libraries(hawaii-desktop ${systemd_LIBRARIES})
endif()

install(TARGETS hawaii-desktop DESTINATION ${CMAKE_INSTALL_LIBDIR}/weston)
