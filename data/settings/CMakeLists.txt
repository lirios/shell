configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/hawaiisettings.kcfg.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hawaiisettings.kcfg"
    IMMEDIATE @ONLY
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/hawaiisettings.kcfgc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hawaiisettings.kcfgc"
    IMMEDIATE @ONLY
)
