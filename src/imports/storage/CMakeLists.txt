# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

find_package(KF6Solid REQUIRED)

ecm_add_qml_module(StorageQmlPlugin
    URI Liri.Storage
    VERSION 1.0
    CLASS_NAME StoragePlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(StorageQmlPlugin
    PRIVATE
        plugin.cpp
        storagedevice.cpp storagedevice.h
        storagemodel.cpp storagemodel.h
)

target_link_libraries(StorageQmlPlugin
    PRIVATE
        Qt6::Qml
        Qt6::Quick
        KF6::Solid
)

ecm_finalize_qml_module(StorageQmlPlugin)

