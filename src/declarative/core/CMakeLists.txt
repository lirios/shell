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
    packagesmodel.cpp
    packagesmodelitem.cpp
)

set(FILES
    qmldir
)

add_library(hawaiishellcoreplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellcoreplugin Qml Quick)
target_link_libraries(hawaiishellcoreplugin HF1HawaiiShell)

install(TARGETS hawaiishellcoreplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Core)
install(FILES ${FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Core)
