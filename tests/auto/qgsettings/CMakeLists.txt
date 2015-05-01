include_directories(
    ${CMAKE_SOURCE_DIR}/libhawaii
)

add_definitions(-DTEST_DATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")

set(tst_qgsettings_SRCS
    tst_qgsettings.cpp
)
add_executable(tst_qgsettings ${tst_qgsettings_SRCS})
target_link_libraries(tst_qgsettings Qt5::Test Hawaii::Hawaii)
add_test(hawaii-test-qgsettings tst_qgsettings)
ecm_mark_as_test(tst_qgsettings)
