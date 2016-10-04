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

add_executable(liri-shell-helper ${SOURCES})
target_link_libraries(liri-shell-helper Qt5::Gui GreenIsland::Client)

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h liri-shell-helper)

install(TARGETS liri-shell-helper DESTINATION ${LIBEXEC_INSTALL_DIR})
