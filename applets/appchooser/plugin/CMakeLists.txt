add_definitions(
  -DQT_USE_QSTRINGBUILDER
  -DQT_NO_CAST_TO_ASCII
# -DQT_NO_CAST_FROM_ASCII
  -DQT_STRICT_ITERATORS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DTRANSLATION_DOMAIN=\"org.hawaii.appchooser\"
)

include_directories(plasmaquick)

set(appchooserplugin_SRCS
    abstractentry.cpp
    abstractmodel.cpp
    actionlist.cpp
    appsmodel.cpp
    containmentinterface.cpp
    draghelper.cpp
    favoritesmodel.cpp
    funnelmodel.cpp
    imagepicker.cpp
    appchooserplugin.cpp
    processrunner.cpp
    rootmodel.cpp
    runnermodel.cpp
    runnermatchesmodel.cpp
    recentappsmodel.cpp
    recentdocsmodel.cpp
    submenu.cpp
    systemmodel.cpp
    windowsystem.cpp
)

install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/hawaii/appchooser/private)

add_library(appchooserplugin SHARED ${appchooserplugin_SRCS})

target_link_libraries(appchooserplugin
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Qml
                      Qt5::Quick
                      Qt5::Widgets # FIXME: Used for QFileDialog; can be removed when switching to Qt Quick Dialogs (see bug 334963).
                      KF5::ConfigCore
                      KF5::CoreAddons
                      KF5::I18n
                      KF5::KDELibs4Support # FIXME: New Solid power management API doesn't exist yet, so we need to use deprecated stuff.
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::PlasmaQuick
                      KF5::Runner
                      KF5::Service
                      KF5::Solid
                      KF5::WindowSystem
                      PW::KWorkspace)

install(TARGETS appchooserplugin DESTINATION ${QML_INSTALL_DIR}/org/hawaii/appchooser/private)
