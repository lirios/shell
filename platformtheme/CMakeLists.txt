include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    hawaiitheme.cpp
)

add_library(hawaii SHARED ${SOURCES})
set_target_properties(hawaii PROPERTIES PREFIX "")
qt5_use_modules(hawaii Gui)

install(TARGETS hawaii LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/plugins/platformthemes)
