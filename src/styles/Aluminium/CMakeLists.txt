set(MODULE_FILES
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
    DESTINATION ${CMAKE_INSTALL_STYLESDIR}/Aluminium)
