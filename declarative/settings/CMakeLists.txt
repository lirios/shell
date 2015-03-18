include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    configgroup.cpp
    plugin.cpp
)

add_library(settingsplugin SHARED ${SOURCES})
target_link_libraries(settingsplugin
                      Qt5::Core
                      Qt5::Qml
                      Qt5::Quick
                      KF5::CoreAddons
                      KF5::ConfigCore)

install(FILES qmldir plugins.qmltypes
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/settings)
install(TARGETS settingsplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/settings)
