add_executable(utils_utest UtilsTest.cpp)
target_link_libraries(utils_utest PUBLIC Catch2::Catch2WithMain PRIVATE webserv-dev)

include(Catch)
catch_discover_tests(utils_utest WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
