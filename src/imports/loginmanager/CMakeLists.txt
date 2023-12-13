# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET Liri::Qt6AccountsService)
    find_package(Qt6AccountsService REQUIRED)
endif()

ecm_add_qml_module(LoginManagerQmlPlugin
    URI Liri.LoginManager
    VERSION 1.0
    CLASS_NAME LoginManagerPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(LoginManagerQmlPlugin
    PRIVATE
        plugin.cpp
        usersmodel.cpp usersmodel.h
)

target_link_libraries(LoginManagerQmlPlugin
    PRIVATE
        Qt6::Qml
        Qt6::Quick
        Liri::Qt6AccountsService
)

ecm_finalize_qml_module(LoginManagerQmlPlugin)
