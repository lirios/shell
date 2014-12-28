include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    hawaiitheme.cpp
)

add_library(HawaiiPlatformTheme SHARED ${SOURCES})
set_target_properties(HawaiiPlatformTheme PROPERTIES PREFIX "")
target_link_libraries(HawaiiPlatformTheme Qt5::Gui)

install(TARGETS HawaiiPlatformTheme LIBRARY
    DESTINATION ${QT_PLUGIN_INSTALL_DIR}/platformthemes)
