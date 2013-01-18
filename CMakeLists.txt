cmake_minimum_required (VERSION 2.8)
project ( jsqlite )

find_package ( Boost COMPONENTS variant)

add_subdirectory ( JSON-CC )

include_directories ( JSON-CC sqlite3 ${Boost_INCLUDE_DIRS})

add_executable ( test test.cpp sqlite3/sqlite3.c)
target_link_libraries ( test json pthread dl)
