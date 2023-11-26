find_package(Catch2 3 REQUIRED)
cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(srcs/incs)
include_directories(srcs/incs/parser)

macro(add_recursive retVal dir ext excl)
    # Store unfiltered list of files into temporary list
    file( GLOB_RECURSE _tmp_list ${dir}/${ext})
    # Resulted list is initially empty. Copy all needed elements into it.
    set(${retval} "")
    foreach(f ${_tmp_list})
        if(NOT f MATCHES ${excl} AND NOT f MATCHES "main.cpp" )
            list(APPEND ${retVal} ${f})
        endif()
    endforeach()
endmacro( add_recursive )

add_recursive(SRCS "srcs" "*.cpp" "/test/")

file( GLOB_RECURSE TEST_SRCS srcs/test/unit/*.cpp)
add_library(webserv-dev ${SRCS})
add_executable(webserv srcs/main.cpp)
target_link_libraries(webserv webserv-dev)
add_subdirectory(srcs/test/unit)
