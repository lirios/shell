include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${QtConfiguration_INCLUDE_DIRS}
    ${HawaiiShell_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    dialogquickwindow_p.cpp
    dialogwindow.cpp
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
set_target_properties(hawaii-notifications-daemon PROPERTIES
    LINK_FLAGS -L${WaylandClient_LIBDIR}
)
target_link_libraries(hawaiishellcomponentsplugin
    ${WaylandClient_LIBRARIES}
    ${HawaiiShell_LIBRARIES}
)

install(TARGETS hawaiishellcomponentsplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Components)
install(FILES ${FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Components)
