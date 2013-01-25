cmake_minimum_required (VERSION 2.8)
project (jl_sqlite3)

include_directories ( ${PROJECT_SOURCE_DIR} )

add_library ( jl_sqlite3 SHARED sqlite3.c)
target_link_libraries ( jl_sqlite3 dl)

add_library ( jl_sqlite3_regexp SHARED regexp.c )
target_link_libraries ( jl_sqlite3_regexp jl_sqlite3 pcre)

