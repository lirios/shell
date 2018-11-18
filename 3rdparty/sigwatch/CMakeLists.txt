add_library(sigwatch STATIC
    sigwatch.cpp
    sigwatch.h
    sigwatch_p.h
)
add_library("Liri::SigWatch" ALIAS "sigwatch")
target_include_directories("sigwatch" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries("sigwatch" Qt5::Core)
