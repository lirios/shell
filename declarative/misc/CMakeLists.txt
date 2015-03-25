set(SOURCES
    keyeventfilter.cpp
    miscplugin.cpp
    standardpaths.cpp
)

add_library(miscplugin SHARED ${SOURCES})
target_link_libraries(miscplugin
                      Qt5::Quick)

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/misc)
install(TARGETS miscplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/misc)
