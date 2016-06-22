include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES materialdecoration.cpp)

qt5_add_resources(RESOURCES icons.qrc)

add_library(HawaiiMaterialDecoration SHARED ${SOURCES} ${RESOURCES})
add_library(Hawaii::MaterialDecoration ALIAS HawaiiMaterialDecoration)
target_link_libraries(HawaiiMaterialDecoration
                      Qt5::Core
                      Qt5::Gui
                      Qt5::WaylandClient)

install(TARGETS HawaiiMaterialDecoration
        LIBRARY DESTINATION ${QT_PLUGIN_INSTALL_DIR}/wayland-decoration-client)

message("${Qt5WaylandClient_INCLUDE_DIRS} ${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS}")
