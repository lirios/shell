include_directories(
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    main.cpp
)

add_executable(hawaii ${SOURCES})
target_link_libraries(hawaii
    Qt5::Core
    Qt5::Gui
    GreenIsland::GreenIsland
    HawaiiSigWatch
)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii)

install(TARGETS hawaii DESTINATION ${BIN_INSTALL_DIR})
