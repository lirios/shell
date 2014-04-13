include_directories(
    ${ALSA_INCLUDE_DIR}
    ${Hawaii_INCLUDE_DIRS}
)

add_definitions(-DQT_PLUGIN)

set(SOURCES
    mixerprovider.cpp
    mixersource.cpp
)

add_library(hawaiimixer SHARED ${SOURCES})
qt5_use_modules(hawaiimixer Core)
target_link_libraries(hawaiimixer
    ${ALSA_LIBRARIES}
    Hawaii
)

install(TARGETS hawaiimixer LIBRARY
    DESTINATION ${CMAKE_INSTALL_DATAPROVIDERSDIR})
