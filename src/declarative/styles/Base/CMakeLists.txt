set(MODULE_FILES
    qmldir
    AppChooserButtonStyle.qml
    DialogStyle.qml
    LauncherStyle.qml
    NotificationBubbleStyle.qml
    OverlayStyle.qml
    PanelIndicatorStyle.qml
    PanelStyle.qml
    PopupStyle.qml
    TooltipStyle.qml
)
install(FILES ${MODULE_FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles/Base)
