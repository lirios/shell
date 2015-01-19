include_directories(
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    main.cpp
    processcontroller.cpp
    processlauncher.cpp
    sessionadaptor.cpp
    sessionmanager.cpp
    powermanager/powermanager.cpp
    powermanager/powermanager.h
    powermanager/powermanagerbackend.cpp
    powermanager/powermanagerbackend.h
    powermanager/systemdpowerbackend.cpp
    powermanager/systemdpowerbackend.h
    powermanager/upowerpowerbackend.cpp
    powermanager/upowerpowerbackend.h
)

add_executable(hawaii-session ${SOURCES})
target_link_libraries(hawaii-session
    Qt5::Core
    Qt5::DBus
    HawaiiQtXDG
    HawaiiSigWatch
    HawaiiQLogind
)

install(TARGETS hawaii-session DESTINATION ${BIN_INSTALL_DIR})
