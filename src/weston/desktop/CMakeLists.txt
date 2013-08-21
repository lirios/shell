include_directories(
    ${WaylandServer_INCLUDE_DIRS}
    ${Pixman_INCLUDE_DIRS}
    ${Weston_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    animation.cpp
    desktop-shell.cpp
    effect.cpp
    inputpanel.cpp
    layer.cpp
    shell.cpp
    shellseat.cpp
    shellsurface.cpp
    transform.cpp
    workspace.cpp
    effects/fademovingeffect.cpp
    effects/griddesktops.cpp
    effects/inoutsurfaceeffect.cpp
    effects/minimizeeffect.cpp
    effects/scaleeffect.cpp
    effects/zoomeffect.cpp
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop-shell
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/desktop-shell.xml
    desktop-shell
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

install(TARGETS hawaii-desktop DESTINATION ${CMAKE_INSTALL_LIBDIR}/weston)
