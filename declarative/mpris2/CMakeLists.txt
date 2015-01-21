set(SOURCES
    mpris2engine.cpp
    mpris2player.cpp
    plugin.cpp
)

set_source_files_properties(
    org.freedesktop.DBus.Properties.xml
    org.mpris.MediaPlayer2.Player.xml
    org.mpris.MediaPlayer2.xml
      PROPERTIES
        NO_NAMESPACE ON
)
qt5_add_dbus_interface(SOURCES org.freedesktop.DBus.Properties.xml dbuspropertiesadaptor)
qt5_add_dbus_interface(SOURCES org.mpris.MediaPlayer2.Player.xml mprisplayeradaptor)
qt5_add_dbus_interface(SOURCES org.mpris.MediaPlayer2.xml mprisadaptor)

add_library(mpris2plugin SHARED ${SOURCES})
target_link_libraries(mpris2plugin
                      Qt5::DBus
                      Qt5::Qml)

install(FILES qmldir plugins.qmltypes
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/mpris2)
install(TARGETS mpris2plugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/mpris2)
