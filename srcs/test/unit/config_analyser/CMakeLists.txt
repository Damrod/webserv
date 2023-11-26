add_executable(config_analyser_utest LexerTest.cpp ParserTest.cpp PreprocessorTest.cpp)
target_link_libraries(config_analyser_utest PUBLIC Catch2::Catch2WithMain PRIVATE webserv-dev)

include(Catch)
catch_discover_tests(config_analyser_utest WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
