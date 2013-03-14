cmake_minimum_required (VERSION 2.8)
project (jl_sqlite3 C)

add_definitions ( -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS )

find_package ( PCRE REQUIRED )
include_directories ( ${PROJECT_SOURCE_DIR} ${PCRE_INCLUDE_DIRS} )

add_library ( jl_sqlite3 SHARED sqlite3.c)
target_link_libraries ( jl_sqlite3 dl)

add_library ( jl_sqlite3_regexp SHARED regexp.c )
target_link_libraries ( jl_sqlite3_regexp jl_sqlite3 ${PCRE_LIBRARIES} )

add_executable ( jl_shell shell.c )
target_link_libraries ( jl_shell jl_sqlite3 pthread)

install ( TARGETS jl_sqlite3 jl_sqlite3_regexp DESTINATION lib )
install ( TARGETS jl_shell DESTINATION bin )
