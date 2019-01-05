if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()

liri_add_qml_plugin(launcher
    MODULE_PATH
        "Liri/Launcher"
    VERSION
        "1.0"
    SOURCES
        application.cpp
        application.h
        applicationmanager.cpp
        applicationmanager.h
        appusage.cpp
        appusage.h
        categoriesmodel.cpp
        categoriesmodel.h
        frequentmodel.cpp
        frequentmodel.h
        launchermodel.cpp
        launchermodel.h
        pagemodel.cpp
        pagemodel.h
        plugin.cpp
        processrunner.cpp
        processrunner.h
        usagetracker.cpp
        usagetracker.h
        utils.cpp
        utils.h
    QML_FILES
        qmldir
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Xml
        Qt5::Sql
        Qt5::WaylandCompositor
        Liri::Xdg
        Liri::Qt5GSettings
)
