include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${QtConfiguration_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/includes
    ${CMAKE_BINARY_DIR}/src/libhawaiishell
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    dialogquickwindow_p.cpp
    dialogwindow.cpp
    overlayquickwindow_p.cpp
    overlaywindow.cpp
    plugin.cpp
    popupquickwindow_p.cpp
    popupwindow.cpp
    registrylistener.cpp
    tooltipwindow.cpp
)

set(FILES
    qmldir
    Dialog.qml
    Popup.qml
    Overlay.qml
    Tooltip.qml
)

wayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)
qtwayland_add_protocol_client(SOURCES
    ${CMAKE_SOURCE_DIR}/data/protocols/hawaii.xml
    hawaii
)

add_library(hawaiishellcomponentsplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellcomponentsplugin Qml Quick)
target_link_libraries(hawaiishellcomponentsplugin HF1HawaiiShell)

install(TARGETS hawaiishellcomponentsplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Components)
install(FILES ${FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Components)
