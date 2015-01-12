#
# You can run Hawaii on:
#
#  - KMS (this requires Qt >= 5.5) and root privileges
#  - Nested into Weston >= 1.5
#
# To make Hawaii run on KMS:
#
# systemctl --user enable hawaii-kms.service
#
# To make Hawaii run nested into Weston:
#
# systemctl --user enable hawaii-nested.service
#
# To start a Hawaii shell upon login:
#
# systemctl --user enable hawaii.target
#
# Otherwise to run it on demand:
#
# systemctl --user isolate hawaii.target
#

configure_file(
    hawaii-kms.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-kms.service
)
configure_file(
    hawaii-nested.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-nested.service
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-kms.service
    ${CMAKE_CURRENT_BINARY_DIR}/hawaii-nested.service
    hawaii.target
)

install(FILES ${FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/user)
