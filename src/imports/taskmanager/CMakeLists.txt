if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::AuroraClient)
    find_package(Liri1AuroraClient REQUIRED)
endif()

liri_add_qml_plugin(TaskManagerQmlPlugin
    MODULE_PATH
        "Liri/Shell/TaskManager"
    VERSION
        "1.0"
    OUTPUT_NAME
        "taskmanagerplugin"
    SOURCES
        application.cpp application.h
        applicationsmodel.cpp applicationsmodel.h
        appusage.cpp appusage.h
        categoriesmodel.cpp categoriesmodel.h
        frequentmodel.cpp frequentmodel.h
        pagemodel.cpp pagemodel.h
        plugin.cpp
        tasksmodel.cpp tasksmodel.h
        usagetracker.cpp usagetracker.h
        xdgutils.cpp xdgutils.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Concurrent
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Sql
        Liri::Qt5GSettings
        Liri::Xdg
        Liri::AuroraClient
)

liri_finalize_qml_plugin(TaskManagerQmlPlugin)
