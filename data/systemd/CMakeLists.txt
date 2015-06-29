#
# You can run Hawaii on:
#
#  - KMS (this requires Qt >= 5.5) and root privileges
#  - Nested into Weston >= 1.5
#
# To start a Hawaii shell upon login:
#
# systemctl --user enable hawaii.service
# systemctl --user enable hawaii.target
#
# Otherwise to run it on demand:
#
# systemctl --user isolate hawaii.target
#

configure_file(
    hawaii.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii.service
)

configure_file(
    hawaii-session.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session.service
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii.service
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-session.service
    hawaii.target
)

install(FILES ${FILES} DESTINATION ${SYSTEMD_USER_UNIT_DIR})
