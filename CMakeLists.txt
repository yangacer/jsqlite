cmake_minimum_required (VERSION 2.8)
project ( jsqlite )

include (use_c++11.cmake)

find_package ( Boost COMPONENTS variant)

add_subdirectory ( JSON-CC )
add_subdirectory ( jl_sqlite3 )

include_directories ( JSON-CC js_sqlite3 ${Boost_INCLUDE_DIRS})

add_library ( jsqlite jsqlite.cpp gen.cpp )
target_link_libraries ( jsqlite json pthread jl_sqlite3)

add_executable ( demo demo.cpp )
target_link_libraries ( demo jsqlite )
