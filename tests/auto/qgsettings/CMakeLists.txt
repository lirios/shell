include_directories(
    ${CMAKE_SOURCE_DIR}/libhawaii
)

add_definitions(
    -DTEST_DATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data/"
    -DQUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

set(tst_qgsettings_SRCS
    tst_qgsettings.cpp
)
add_executable(tst_qgsettings ${tst_qgsettings_SRCS})
target_link_libraries(tst_qgsettings Qt5::Test Hawaii::Hawaii)
add_test(hawaii-test-qgsettings tst_qgsettings)
ecm_mark_as_test(tst_qgsettings)

set(tst_qmlgsettings_SRCS
    tst_qmlgsettings.cpp
)
add_executable(tst_qmlgsettings ${tst_qmlgsettings_SRCS})
target_link_libraries(tst_qmlgsettings Qt5::Quick Qt5::QuickTest)
add_test(hawaii-test-qmlgsettings tst_qmlgsettings)
ecm_mark_as_test(tst_qmlgsettings)
