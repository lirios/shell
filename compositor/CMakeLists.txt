find_package(PAM REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    application.cpp
    main.cpp
    processlauncher.cpp
    screensaver.cpp
    sessionmanager.cpp
    loginmanager/loginmanager.cpp
    loginmanager/loginmanagerbackend.cpp
    loginmanager/logindbackend.cpp
    loginmanager/fakebackend.cpp
    powermanager/powermanager.cpp
    powermanager/powermanager.h
    powermanager/powermanagerbackend.cpp
    powermanager/powermanagerbackend.h
    powermanager/systemdpowerbackend.cpp
    powermanager/systemdpowerbackend.h
    powermanager/upowerpowerbackend.cpp
    powermanager/upowerpowerbackend.h
    session/authenticator.cpp
    session/sessioninterface.cpp
)

qt5_add_dbus_adaptor(SOURCES org.hawaiios.ProcessLauncher.xml
                     processlauncher.h ProcessLauncher
                     processlauncheradaptor ProcessLauncherAdaptor)
qt5_add_dbus_adaptor(SOURCES org.freedesktop.ScreenSaver.xml
                     screensaver.h ScreenSaver
                     screensaveradaptor ScreenSaverAdaptor)

add_executable(hawaii ${SOURCES})
target_link_libraries(hawaii
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    Qt5::Widgets
    GreenIsland::GreenIsland
    HawaiiSigWatch
    ${QTXDG_LIBRARIES}
    ${PAM_LIBRARIES}
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii)

install(TARGETS hawaii DESTINATION ${BIN_INSTALL_DIR})
install(FILES data/eglfs_kms.json DESTINATION ${DATA_INSTALL_DIR}/hawaii/compositor/eglfs)
