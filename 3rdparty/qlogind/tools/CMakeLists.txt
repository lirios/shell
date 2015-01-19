add_executable (enhancedqdbusxml2cpp
                enhancedqdbusxml2cpp.cpp)

target_link_libraries (enhancedqdbusxml2cpp
    Qt5::DBus
    Qt5::Core
)
