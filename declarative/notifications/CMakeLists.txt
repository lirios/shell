include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    notifications.cpp
    notificationsdaemon.cpp
    notificationsimage.cpp
    notificationsimageprovider.cpp
    plugin.cpp
)

qt5_add_dbus_adaptor(SOURCES
    org.freedesktop.Notifications.xml
    notificationsdaemon.h NotificationsDaemon)

add_library(notificationsplugin SHARED ${SOURCES})
target_link_libraries(notificationsplugin
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Gui
                      Qt5::Qml
                      Qt5::Quick)

install(FILES qmldir plugins.qmltypes
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/notifications)
install(TARGETS notificationsplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/notifications)
