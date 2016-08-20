set(FILES
    qmldir
    Background.qml
    DateTime.qml
    DateTimeIndicator.qml
    Indicator.qml
    LoginGreeter.qml
    PanelItem.qml
    UserDelegate.qml
)

install(FILES ${FILES}
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Desktop)
