include_directories(
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

add_library(LiriSigWatch STATIC sigwatch.cpp)
target_link_libraries(LiriSigWatch Qt5::Core)
