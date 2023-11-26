add_executable(http_utest FormFileTest.cpp HttpRequestTest.cpp HttpResponseTest.cpp MimeTypesTest.cpp)
target_link_libraries(http_utest PUBLIC Catch2::Catch2WithMain PRIVATE webserv-dev)

include(Catch)
catch_discover_tests(http_utest WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
