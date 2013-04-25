cmake_minimum_required (VERSION 2.8)
project ( jsqlite )


list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_mod )
include (use_c++11)

find_package ( JSONCC REQUIRED )
find_package ( Boost )

add_subdirectory ( jl_sqlite3 )

include_directories ( js_sqlite3 ${Boost_INCLUDE_DIRS} ${JSONCC_INCLUDE_DIRS} )

add_library ( jsqlite SHARED jsqlite.cpp gen.cpp ) #where.cpp)
target_link_libraries ( jsqlite ${JSONCC_LIBRARIES} pthread jl_sqlite3)

add_library ( utf8tok SHARED utf8_tokenizer.cpp )
target_link_libraries ( jl_sqlite3 )

add_library ( unihan SHARED unihan.cpp )

add_executable ( demo demo.cpp )
target_link_libraries ( demo jsqlite )

add_subdirectory ( tests )

install ( TARGETS jsqlite utf8tok unihan DESTINATION lib )
install ( FILES Unihan_Variants_reduce.txt DESTINATION share )
