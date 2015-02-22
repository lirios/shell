#generate cpp from xml files

macro(generateInterface interfaceName)
    string(REPLACE "." "" fileName ${interfaceName})

    add_custom_command(OUTPUT ${fileName}.cpp OUTPUT ${fileName}.h
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../tools/enhancedqdbusxml2cpp -p ${fileName} ${CMAKE_SOURCE_DIR}/3rdparty/qlogind/spec/${interfaceName}.xml -i types.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Interface for ${interfaceName}"
        SOURCES $fileName.h
    )
    qt_wrap_cpp(qlogind mocfile ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h)
    list(APPEND generatedInterfaces ${fileName}.cpp)
    list(APPEND generatedInterfaces ${mocfile})
    list(APPEND generatedHeaders ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h)

endmacro(generateInterface)

generateInterface("org.freedesktop.login1.Manager")
generateInterface("org.freedesktop.login1.Session")
generateInterface("org.freedesktop.login1.Seat")
generateInterface("org.freedesktop.login1.User")


add_library(HawaiiQLogind
            STATIC
            pendinginterface.cpp
            sessiontracker.cpp
            usertracker.cpp
            seattracker.cpp
            manager.cpp
            session.cpp
            seat.cpp
            user.cpp
            types.cpp
            enhancedqdbusabstractinterface.cpp
            ${generatedInterfaces}
            )

generate_export_header(HawaiiQLogind BASE_NAME QLogind EXPORT_FILE_NAME qlogind_export.h)

target_link_libraries (HawaiiQLogind
    Qt5::DBus
    Qt5::Core
)
