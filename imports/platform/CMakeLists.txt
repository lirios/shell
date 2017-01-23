file(GLOB_RECURSE SOURCES *.cpp *.h)
file(GLOB_RECURSE QML_FILES *.qml)

include_directories(SYSTEM
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS}
)

add_library(liriplatform SHARED ${SOURCES})
target_link_libraries(liriplatform
                      Qt5::Core
                      Qt5::Qml
                      Qt5::Quick
                      Qt5::Gui
                      Qt5::WaylandClient
                      Liri::MaterialDecoration)

install(FILES qmldir ${QML_FILES}
    DESTINATION ${QML_INSTALL_DIR}/Liri/Platform)
install(TARGETS liriplatform
    DESTINATION ${QML_INSTALL_DIR}/Liri/Platform)
