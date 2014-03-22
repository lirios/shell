include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/includes
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/applications
    ${CMAKE_BINARY_DIR}/src/libhawaiishell
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
    applicationiconprovider.cpp
    applicationsmodel.cpp
)

set(FILES
    qmldir
)

add_library(hawaiishellapplicationsplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellapplicationsplugin Qml Quick)
target_link_libraries(hawaiishellapplicationsplugin HF1HawaiiShell)

install(TARGETS hawaiishellapplicationsplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Applications)
install(FILES ${FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Applications)
