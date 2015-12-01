find_package(PAM REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    main.cpp
    processlauncher/processlauncher.cpp
    screensaver/screensaver.cpp
    sessionmanager/authenticator.cpp
    sessionmanager/sessioninterface.cpp
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
)

qt5_add_dbus_adaptor(SOURCES processlauncher/org.hawaiios.ProcessLauncher.xml
                     processlauncher/processlauncher.h ProcessLauncher
                     processlauncheradaptor ProcessLauncherAdaptor)
qt5_add_dbus_adaptor(SOURCES screensaver/org.freedesktop.ScreenSaver.xml
                     screensaver/screensaver.h ScreenSaver
                     screensaveradaptor ScreenSaverAdaptor)

add_executable(hawaii ${SOURCES})
target_link_libraries(hawaii
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::Widgets
    GreenIsland::Server
    HawaiiSigWatch
    Qt5Xdg
    ${PAM_LIBRARIES}
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii)

install(TARGETS hawaii DESTINATION ${BIN_INSTALL_DIR})
