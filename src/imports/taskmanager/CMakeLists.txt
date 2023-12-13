if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
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
        wlrforeigntoplevelmanagementv1.cpp wlrforeigntoplevelmanagementv1.h wlrforeigntoplevelmanagementv1_p.h
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
        Qt5::WaylandClient
        Qt5::Sql
        Liri::Qt5GSettings
        Liri::Xdg
)

ecm_qt_declare_logging_category(
    TaskManagerQmlPlugin
    HEADER "lirishelltaskmanagerlogging.h"
    IDENTIFIER "gLcTaskManager"
    CATEGORY_NAME "liri.shell.taskmanager"
    DEFAULT_SEVERITY "Info"
)

aurora_generate_wayland_protocol_client_sources(TaskManagerQmlPlugin
    FILES
        wlr-foreign-toplevel-management-unstable-v1.xml
)

liri_finalize_qml_plugin(TaskManagerQmlPlugin)
