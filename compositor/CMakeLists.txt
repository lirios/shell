include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    application.cpp
    main.cpp
)

add_executable(hawaii ${SOURCES})
target_link_libraries(hawaii
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Widgets
    GreenIsland::GreenIsland
    HawaiiSigWatch
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii)

install(TARGETS hawaii DESTINATION ${BIN_INSTALL_DIR})
