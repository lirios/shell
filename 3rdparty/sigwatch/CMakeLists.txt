set(SOURCES
    xdgautostart.cpp
    xdgdesktopfile.cpp
    xdgdirs.cpp
    desktopenvironment_p.cpp
)

add_library(HawaiiSigWatch STATIC sigwatch.cpp)
target_link_libraries(HawaiiSigWatch Qt5::Core)
