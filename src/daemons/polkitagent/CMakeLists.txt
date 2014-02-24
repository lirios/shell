include_directories(
    ${POLKITQT-1_INCLUDE_DIR}
    ${QtAccountsService_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

set(SOURCES
    main.cpp
    policykitagent.cpp
)

set(LINK_FLAGS "-L${POLKITQT-1_LIB_DIR}")
set(LIBRARIES
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${QtAccountsService_LIBRARIES}
)
if(systemd_FOUND)
    set(LINK_FLAGS "${LINK_FLAGS} -L${systemd_LIBDIR}")
    set(LIBRARIES ${LIBRARIES} ${systemd_LIBRARIES})
endif()

add_executable(hawaii-polkit-agent ${SOURCES} ${RESOURCES})
qt5_use_modules(hawaii-polkit-agent DBus Quick)
set_target_properties(hawaii-polkit-agent PROPERTIES LINK_FLAGS ${LINK_FLAGS})
target_link_libraries(hawaii-polkit-agent ${LIBRARIES})

install(TARGETS hawaii-polkit-agent DESTINATION ${CMAKE_INSTALL_BINDIR})
