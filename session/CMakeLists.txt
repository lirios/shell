include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_SOURCE_DIR}/3rdparty
)

set(SOURCES
    main.cpp
    sessionadaptor.cpp
)

add_executable(hawaii-session ${SOURCES})
target_link_libraries(hawaii-session
    Qt5::Core
    Qt5::DBus
    HawaiiSigWatch
)

target_link_libraries(hawaii-session ${QTXDG_LIBRARIES})

# Git revision
include(GetGitRevision)
create_git_head_revision_file(gitsha1.h hawaii-session)

install(TARGETS hawaii-session DESTINATION ${BIN_INSTALL_DIR})
