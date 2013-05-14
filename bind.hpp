#ifndef JSQLITE_BIND_HPP_
#define JSQLITE_BIND_HPP_

#include <string>
#include "json/variant.hpp"
#include "jl_sqlite3/sqlite3.h"

namespace jsqlite {
namespace json = yangacer::json;

struct argument
{
  char const *value;
  size_t size;
  size_t seq_num;
};

int select_bind_text(json::array_t &result,
                sqlite3* db,
                std::string const &stmt,
                // char **error,
                argument const *arg,
                size_t arg_num);

} // namespace jsqlite

#endif
