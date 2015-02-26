set(MODULE_FILES
    qmldir
    BusyIndicatorStyle.qml
    ButtonStyle.qml
    CalendarStyle.qml
    FocusFrameStyle.qml
    GroupBoxStyle.qml
    MenuStyle.qml
    ProgressBarStyle.qml
    ScrollViewStyle.qml
    SliderStyle.qml
    TextFieldStyle.qml
    ToolButtonStyle.qml
)
install(FILES ${MODULE_FILES}
    DESTINATION ${QML_INSTALL_DIR}/QtQuick/Controls/Styles/Wind)

set(IMAGE_FILES
    images/arrow-down.png
    images/arrow-down@2x.png
    images/arrow-left.png
    images/arrow-left@2x.png
    images/arrow-right.png
    images/arrow-right@2x.png
    images/arrow-up.png
    images/arrow-up@2x.png
    images/focusframe.png
    images/noise-texture.png
    images/progress-indeterminate.png
)
install(FILES ${IMAGE_FILES}
    DESTINATION ${QML_INSTALL_DIR}/QtQuick/Controls/Styles/Wind/images)
