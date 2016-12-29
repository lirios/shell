find_package(PAM REQUIRED)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../headers"
    "${CMAKE_CURRENT_BINARY_DIR}/../headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty"
    "${CMAKE_CURRENT_BINARY_DIR}/sessionmanager/screensaver"
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

set(SOURCES
    application.cpp
    main.cpp
    processlauncher/processlauncher.cpp
    sessionmanager/authenticator.cpp
    sessionmanager/sessionmanager.cpp
    sessionmanager/loginmanager/loginmanager.cpp
    sessionmanager/loginmanager/loginmanagerbackend.cpp
    sessionmanager/loginmanager/logindbackend.cpp
    sessionmanager/loginmanager/fakebackend.cpp
    sessionmanager/powermanager/powermanager.cpp
    sessionmanager/powermanager/powermanager.h
    sessionmanager/powermanager/powermanagerbackend.cpp
    sessionmanager/powermanager/powermanagerbackend.h
    sessionmanager/powermanager/systemdpowerbackend.cpp
    sessionmanager/powermanager/systemdpowerbackend.h
    sessionmanager/powermanager/upowerpowerbackend.cpp
    sessionmanager/powermanager/upowerpowerbackend.h
    sessionmanager/screensaver/screensaver.cpp
)

qt5_add_dbus_adaptor(SOURCES processlauncher/io.liri.ProcessLauncher.xml
                     processlauncher/processlauncher.h ProcessLauncher
                     processlauncheradaptor ProcessLauncherAdaptor)
qt5_add_dbus_adaptor(SOURCES sessionmanager/screensaver/org.freedesktop.ScreenSaver.xml
                     sessionmanager/screensaver/screensaver.h ScreenSaver
                     sessionmanager/screensaver/screensaveradaptor ScreenSaverAdaptor)

qt5_add_resources(RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/qml/shell.qrc")

add_executable(liri-shell ${SOURCES} ${RESOURCES})
target_link_libraries(liri-shell
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::Widgets
    Qt5::QuickControls2
    Liri::WaylandServer
    LiriSigWatch
    Vibe::Settings
    Qt5Xdg
    ${PAM_LIBRARIES}
)

if(systemd_FOUND)
    target_link_libraries(liri-shell ${systemd_LIBRARIES})
endif()

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h liri-shell)

install(TARGETS liri-shell DESTINATION ${BIN_INSTALL_DIR})
