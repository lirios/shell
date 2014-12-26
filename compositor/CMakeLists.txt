include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
)

add_executable(hawaii ${SOURCES})
target_link_libraries(hawaii
    Qt5::Core
    GreenIsland::GreenIsland
)

install(TARGETS hawaii DESTINATION ${BIN_INSTALL_DIR})
