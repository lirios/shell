ecm_add_qml_module(LayerShellQtQmlPlugin
    URI Liri.Shell.LayerShell
    VERSION 1.0
    CLASS_NAME Plugin
    NO_GENERATE_PLUGIN_SOURCE
)

target_sources(LayerShellQtQmlPlugin PRIVATE layershellqtplugin.cpp)

target_link_libraries(LayerShellQtQmlPlugin PRIVATE Qt6::Qml LayerShellQt::Interface)

ecm_finalize_qml_module(LayerShellQtQmlPlugin)
