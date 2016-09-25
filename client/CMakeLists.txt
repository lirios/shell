include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/../headers"
    "${CMAKE_CURRENT_BINARY_DIR}/../headers"
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    shellhelperapplication.cpp
    shellhelperclient.cpp
)

greenisland_add_client_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../data/protocols/shell-helper.xml"
    BASENAME shell-helper
)

add_executable(hawaii-client ${SOURCES})
target_link_libraries(hawaii-client Qt5::Gui GreenIsland::Client)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii-client)

install(TARGETS hawaii-client DESTINATION ${LIBEXEC_INSTALL_DIR})
