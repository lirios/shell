include_directories(
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

add_library(HawaiiSigWatch STATIC sigwatch.cpp)
target_link_libraries(HawaiiSigWatch Qt5::Core)
