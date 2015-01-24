set(MODULE_FILES
    qmldir
    BusyIndicatorStyle.qml
    ButtonStyle.qml
    FocusFrameStyle.qml
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
    images/noise-texture.png
    images/progress-indeterminate.png
)
install(FILES ${IMAGE_FILES}
    DESTINATION ${QML_INSTALL_DIR}/QtQuick/Controls/Styles/Wind/images)

set(PRIVATE_FILES
    private/Utils.js
)
install(FILES ${PRIVATE_FILES}
    DESTINATION ${QML_INSTALL_DIR}/QtQuick/Controls/Styles/Wind/private)
