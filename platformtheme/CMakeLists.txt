include_directories(
    ${CMAKE_SOURCE_DIR}/libhawaii
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    hawaiitheme.cpp
    hawaiitheme_p.cpp
)

add_library(HawaiiPlatformTheme SHARED ${SOURCES})
set_target_properties(HawaiiPlatformTheme PROPERTIES PREFIX "")
target_link_libraries(HawaiiPlatformTheme Qt5::Gui Hawaii::Hawaii)

install(TARGETS HawaiiPlatformTheme LIBRARY
    DESTINATION ${QT_PLUGIN_INSTALL_DIR}/platformthemes)
