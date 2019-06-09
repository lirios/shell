find_package(Wayland REQUIRED)
find_package(QtWaylandScanner REQUIRED)

ecm_add_qtwayland_server_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/liri-decoration.xml"
    BASENAME "liri-decoration")
ecm_add_qtwayland_server_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/gtk-shell.xml"
    BASENAME "gtk-shell")
ecm_add_qtwayland_server_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/server-decoration.xml"
    BASENAME "server-decoration")
ecm_add_qtwayland_server_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/shell-helper.xml"
    BASENAME "shell-helper")

if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()

liri_add_module(WaylandExtensions
    SOURCES
        gtkshell.cpp
        gtkshell.h
        gtkshell_p.h
        kdeserverdecoration.cpp
        kdeserverdecoration.h
        liridecoration.cpp
        liridecoration.h
        shellhelper.cpp
        shellhelper.h
        shellhelper_p.h
        ${SOURCES}
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        QT_WAYLAND_COMPOSITOR_QUICK
        INSTALL_LIBEXECDIR="${LIBEXECDIR}"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Qml
        Qt5::Quick
        Qt5::WaylandCompositor
        Wayland::Server
    STATIC
    NO_MODULE_HEADERS
    NO_CMAKE
    NO_PKGCONFIG
)
