include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    processcontroller.cpp
    sessionadaptor.cpp
    sessionmanager.cpp
)

add_executable(hawaii-session ${SOURCES})
target_link_libraries(hawaii-session
    Qt5::Core
    Qt5::DBus
)

install(TARGETS hawaii-session DESTINATION ${BIN_INSTALL_DIR})
