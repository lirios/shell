# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET Liri::Qt6GSettings)
    find_package(Qt6GSettings REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()

ecm_add_qml_module(TaskManagerQmlPlugin
    URI Liri.Shell.TaskManager
    VERSION 1.0
    CLASS_NAME TaskManagerPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(TaskManagerQmlPlugin
    PRIVATE
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
)

target_link_libraries(TaskManagerQmlPlugin
    PRIVATE
        Qt6::Core
        Qt6::Concurrent
        Qt6::DBus
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::WaylandClient
        Qt6::Sql
        Liri::Qt6GSettings
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
        ${PROJECT_SOURCE_DIR}/data/protocols/wlr-foreign-toplevel-management-unstable-v1.xml
)

ecm_finalize_qml_module(TaskManagerQmlPlugin)
