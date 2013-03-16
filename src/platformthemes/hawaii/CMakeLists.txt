include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${VibeCore_INCLUDE_DIRS}
    ${VibeGui_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    hawaiitheme.cpp
)

add_library(hawaii SHARED ${SOURCES})
qt5_use_modules(hawaii Gui)
#target_link_libraries(hawaii ${VibeGui_LIBRARIES})

install(TARGETS hawaii LIBRARY DESTINATION ${CMAKE_INSTALL_PLUGINSDIR}/platformthemes)
