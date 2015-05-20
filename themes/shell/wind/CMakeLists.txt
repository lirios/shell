set(FILES
    theme.ini
    Palette.qml
    Popover.qml
    PopupMenu.qml
    Tooltip.qml
)
install(FILES ${FILES}
    DESTINATION ${DATA_INSTALL_DIR}/hawaii/themes/Wind)

set(IMAGES
    images/dropshadow.png
    images/dropshadow.sci
)
install(FILES ${IMAGES}
    DESTINATION ${DATA_INSTALL_DIR}/hawaii/themes/Wind/images)
