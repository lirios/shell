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

set(IMAGE_FILES
    noise.png
)
install(FILES ${IMAGE_FILES}
    DESTINATION ${CMAKE_INSTALL_STYLESDIR}/Aluminium/images)
