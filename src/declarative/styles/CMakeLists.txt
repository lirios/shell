include_directories(
    ${QtConfiguration_INCLUDE_DIRS}
    ${HawaiiShell_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
    abstractstyle.cpp
    padding.cpp
    stylesettings.cpp
)

add_library(hawaiishellstylesplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellstylesplugin Qml)
target_link_libraries(hawaiishellstylesplugin
    ${QtConfiguration_LIBRARIES}
    ${HawaiiShell_LIBRARIES}
)

install(TARGETS hawaiishellstylesplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles)

set(MODULE_FILES
    qmldir
    Style.qml
    StyledItem.qml
)
install(FILES ${MODULE_FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles)

add_subdirectory(Base)
