if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()
if(NOT TARGET Liri::AuroraCompositor)
    find_package(Liri1AuroraCompositor REQUIRED)
endif()

liri_add_qml_plugin(LauncherQmlPlugin
    MODULE_PATH
        "Liri/Launcher"
    VERSION
        "1.0"
    OUTPUT_NAME
        "launcherplugin"
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
        usagetracker.cpp
        usagetracker.h
        utils.cpp
        utils.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Xml
        Qt5::Sql
        Liri::AuroraCompositor
        Liri::Xdg
        Liri::Qt5GSettings
)

liri_finalize_qml_plugin(LauncherQmlPlugin)
