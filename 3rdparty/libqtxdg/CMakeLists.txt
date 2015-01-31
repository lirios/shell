set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_definitions(-DHAVE_QTMIMETYPES)

set(SOURCES
    xdgautostart.cpp
    xdgdesktopfile.cpp
    xdgdirs.cpp
    desktopenvironment_p.cpp
)

add_library(HawaiiQtXDG STATIC ${SOURCES})
target_link_libraries(HawaiiQtXDG Qt5::Core Qt5::Gui)
