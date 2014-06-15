#
# To enable Hawaii components:
#
# systemctl --user enable hawaii-weston.service
# systemctl --user enable hawaii-notifications-daemon.service
# systemctl --user enable hawaii-polkit-agent.service
#
# If you want to use the QML compositor instead of Weston:
#
# systemctl --user disable hawaii-weston.service
# systemctl --user enable hawaii-shell.service
#
# To start a Hawaii shell upon login:
#
# systemctl --user enable hawaii.target
#

configure_file(
    hawaii-notifications-daemon.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-notifications-daemon.service
)
if(PolkitQt5-1_FOUND)
    configure_file(
        hawaii-polkit-agent.service.in
        ${CMAKE_CURRENT_BINARY_DIR}/hawaii-polkit-agent.service
    )
endif()
configure_file(
    hawaii-shell.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-shell.service
)
configure_file(
    hawaii-weston.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-weston.service
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-notifications-daemon.service
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-shell.service
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-weston.service
    hawaii.target
)
if(PolkitQt5-1_FOUND)
    set(FILES
        ${FILES}
        ${CMAKE_CURRENT_BINARY_DIR}/hawaii-polkit-agent.service
    )
endif()

install(FILES ${FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/user)
