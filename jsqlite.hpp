#ifndef JSQLITE_HPP_
#define JSQLITE_HPP_

#include <string>
#include <vector>
#include <ostream>
#include "json/json.hpp"
#include "jl_sqlite3/sqlite3.h"

namespace jsqlite {
namespace json = yangacer::json;

struct transaction
{
  explicit transaction(sqlite3* db, std::ostream &logger);
  ~transaction();
  bool commit();
  bool rollback();
private:
  sqlite3* db_;
  bool finalized_;
  std::ostream &logger_;
  int ec_;
};

/**
 * Select rows from db according to statement
 * @param result 
 * @param db Valid sqlite3 db handle
 * @param stmt Statement
 * @param error Error string reported from sqlite3. It needs to be freed
 * manually.
 * @return SQLite3 error code
 */
int select(json::array_t &result, 
           sqlite3* db, 
           std::string const &stmt,
           char **error);
/** 
 * Generate literal data from variable
 * @param variable
 * @param Literal data
 * @remark Quote and escap will be applied according to
 * type of a variable automatically.
 */
std::string lit(json::var_t const &variable);

/**
 * Generate column names
 * @param object
 * @return "( column_name1, column_name2, ... )"
 */
std::string col_names(json::object_t const &object);

/**
 * Generate column values
 * @param object
 * @return "( column_value1, column_value2, ... )"
 */
std::string col_values(json::object_t const &object);

std::string vector(json::array_t const &array);

} // namespace jsqlite

#endif
