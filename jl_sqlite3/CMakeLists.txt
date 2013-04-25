cmake_minimum_required (VERSION 2.8)
project (jl_sqlite3 C)

add_definitions ( -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS )

find_package ( PCRE REQUIRED )
include_directories ( ${PROJECT_SOURCE_DIR} ${PCRE_INCLUDE_DIRS} )

find_package ( OpenSSL )
find_package ( Boost )

configure_file ( config.h.in ${PROJECT_SOURCE_DIR}/config.h )

add_library ( jl_sqlite3 SHARED sqlite3.c)
#set_target_properties ( jl_sqlite3 PROPERTIES COMPILE_FLAGS "-DSQLITE_ENABLE_ICU")
target_link_libraries ( jl_sqlite3 dl )#icui18n icuio ) #icule iculx icutu icuuc icudata)

add_library ( jl_sqlite3_regexp SHARED regexp.c )
target_link_libraries ( jl_sqlite3_regexp jl_sqlite3 ${PCRE_LIBRARIES} )

if ( OPENSSL_FOUND )
  include_directories ( ${OPENSSL_INCLUDE_DIR} )
  add_library ( jl_sqlite3_md5 SHARED md5.c )
  target_link_libraries ( jl_sqlite3_md5 jl_sqlite3 ${OPENSSL_LIBRARIES} )
  
  add_library ( jl_sqlite3_sha1 SHARED sha.c )
  target_link_libraries ( jl_sqlite3_sha1 jl_sqlite3 ${OPENSSL_LIBRARIES} )
endif()

if ( Boost_FOUND )
  include_directories ( ${Boost_INCLUDE_DIRS} )
  add_library ( jl_sqlite3_unihan SHARED unihan.cpp )
  target_link_libraries ( jl_sqlite3_unihan jl_sqlite3 unihan)
endif()

add_executable ( jl_shell shell.c )
target_link_libraries ( jl_shell jl_sqlite3 pthread)

install ( TARGETS jl_sqlite3 jl_sqlite3_regexp DESTINATION lib )
install ( TARGETS jl_shell DESTINATION bin )

if ( OPENSSL_FOUND )
  install ( TARGETS jl_sqlite3_sha1 jl_sqlite3_md5 DESTINATION lib )
endif()

if ( Boost_FOUND )
  install ( TARGETS jl_sqlite3_unihan DESTINATION lib )
endif()
