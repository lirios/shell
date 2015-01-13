set(SOURCES
    abstractentry.cpp
    abstractmodel.cpp
    actionlist.cpp
    appsmodel.cpp
    favoritesmodel.cpp
    funnelmodel.cpp
    appchooserplugin.cpp
    processrunner.cpp
    rootmodel.cpp
    runnermodel.cpp
    runnermatchesmodel.cpp
    recentappsmodel.cpp
    recentdocsmodel.cpp
    sessioninterface.cpp
    systemmodel.cpp
)

add_library(appchooserplugin SHARED ${SOURCES})
target_link_libraries(appchooserplugin
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Qml
                      Qt5::Quick
                      KF5::KDELibs4Support
                      KF5::ConfigCore
                      KF5::CoreAddons
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::Runner
                      KF5::Service
                      KF5::Solid)

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/appchooser)
install(TARGETS appchooserplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/appchooser)
