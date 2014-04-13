include_directories(
    ${Hawaii_INCLUDE_DIRS}
)

add_definitions(-DQT_PLUGIN)

set(SOURCES
    datetimeprovider.cpp
    datetimesource.cpp
)

add_library(hawaiidatetime SHARED ${SOURCES})
qt5_use_modules(hawaiidatetime Core)
target_link_libraries(hawaiidatetime ${Hawaii_LIBRARIES})

install(TARGETS hawaiidatetime LIBRARY
    DESTINATION ${CMAKE_INSTALL_DATAPROVIDERSDIR})
