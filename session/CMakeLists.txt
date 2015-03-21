include_directories(
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    main.cpp
    processcontroller.cpp
    processlauncher.cpp
    screensaver.cpp
    sessionadaptor.cpp
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
)

qt5_add_dbus_adaptor(SOURCES
    org.freedesktop.ScreenSaver.xml
    screensaver.h ScreenSaver)

add_executable(hawaii-session ${SOURCES})
target_link_libraries(hawaii-session
    Qt5::Core
    Qt5::DBus
    HawaiiSigWatch
)

if(QTXDG_FOUND)
    target_link_libraries(hawaii-session ${QTXDG_LIBRARIES})
else()
    target_link_libraries(hawaii-session HawaiiQtXDG)
endif()

install(TARGETS hawaii-session DESTINATION ${BIN_INSTALL_DIR})
