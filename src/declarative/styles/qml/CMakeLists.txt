set(MODULE_FILES
    qmldir
    Style.qml
    StyledItem.qml
)
install(FILES ${MODULE_FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles)
