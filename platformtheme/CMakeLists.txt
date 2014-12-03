include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    hawaiitheme.cpp
)

add_library(hawaii SHARED ${SOURCES})
set_target_properties(hawaii PROPERTIES PREFIX "")
target_link_libraries(hawaii Qt5::Gui)

install(TARGETS hawaii LIBRARY
    DESTINATION ${PLUGIN_INSTALL_DIR}/platformthemes)
