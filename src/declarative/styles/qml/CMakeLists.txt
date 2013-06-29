set(MODULE_FILES
    qmldir
    Style.qml
    StyledItem.qml
)
install(FILES ${MODULE_FILES}
    DESTINATION ${CMAKE_INSTALL_QMLDIR}/Hawaii/Shell/Styles)
