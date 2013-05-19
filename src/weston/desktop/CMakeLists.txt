include_directories(
    ${WaylandServer_INCLUDE_DIRS}
    ${Pixman_INCLUDE_DIRS}
    ${Weston_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    shell.cpp
    layer.cpp
    shellsurface.cpp
    shellseat.cpp
    workspace.cpp
    effect.cpp
    transform.cpp
    animation.cpp
    desktop-shell.cpp
    effects/scaleeffect.cpp
    effects/griddesktops.cpp
    effects/zoomeffect.cpp
    effects/fademovingeffect.cpp
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
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification-daemon
)
wayland_add_protocol_server(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/notification-daemon.xml
    notification-daemon
)

add_library(hawaii-desktop SHARED ${SOURCES})
set_target_properties(hawaii-desktop PROPERTIES PREFIX "")

install(TARGETS hawaii-desktop DESTINATION ${CMAKE_INSTALL_WESTONLIBDIR}/weston)
