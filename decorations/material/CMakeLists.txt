include_directories(SYSTEM
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES materialdecoration.cpp)

qt5_add_resources(RESOURCES icons.qrc)

add_library(material SHARED ${SOURCES} ${RESOURCES})
add_library(Liri::MaterialDecoration ALIAS material)
target_link_libraries(material
                      Qt5::Core
                      Qt5::Gui
                      Qt5::WaylandClient)

install(TARGETS material
        LIBRARY DESTINATION ${QT_PLUGIN_INSTALL_DIR}/wayland-decoration-client)
