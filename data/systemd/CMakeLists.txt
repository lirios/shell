#
# You can run Liri on:
#
#  - KMS (this requires Qt >= 5.5) and root privileges
#  - Nested into Weston >= 1.5
#
# To start a Liri session upon login:
#
# systemctl --user enable liri.service
# systemctl --user enable liri.target
#
# Otherwise to run it on demand:
#
# systemctl --user isolate liri.target
#

configure_file(
    liri.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/liri.service
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/liri.service
    liri.target
)

install(FILES ${FILES} DESTINATION ${SYSTEMD_USER_UNIT_DIR})
