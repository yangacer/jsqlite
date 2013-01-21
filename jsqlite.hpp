#ifndef JSQLITE_HPP_
#define JSQLITE_HPP_

#include <string>
#include <vector>
#include "json/json.hpp"
#include "js_sqlite3/sqlite3.h"

namespace jsqlite {
namespace json = yangacer::json;

/**
 * Select rows from db according to statement
 * @param result 
 * @param db Valid sqlite3 db handle
 * @param stmt Statement
 * @return SQLite3 error code
 */
int select(std::vector<json::var_t> &result, sqlite3* db, std::string const &stmt);

/** 
 * Generate literal data from variable
 * @param variable
 * @param Literal data
 * @remark Quote and escap will be applied according to
 * type of a variable automatically.
 */
std::string lit(json::var_t const &variable);

std::string lit(json::object_t const &object);

} // namespace jsqlite

#endif
