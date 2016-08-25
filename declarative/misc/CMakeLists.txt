set(SOURCES
    formats.cpp
    kformat.cpp
    kformatprivate.cpp
    miscplugin.cpp
)

add_library(miscplugin SHARED ${SOURCES})
target_link_libraries(miscplugin
                      Qt5::Core
                      Qt5::Quick)

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/misc)
install(TARGETS miscplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/misc)
