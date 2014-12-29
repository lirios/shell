include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
    processcontroller.cpp
)

add_executable(hawaii-session ${SOURCES})
target_link_libraries(hawaii-session
    Qt5::Core
)

install(TARGETS hawaii-session DESTINATION ${BIN_INSTALL_DIR})
