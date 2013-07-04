#include "bind.hpp"
#include "json/json.hpp"

namespace jsqlite {

int select_bind_text(json::array_t &result,
                sqlite3* db,
                std::string const &stmt,
                // TODO Figure out how to get error msg
                // char **error,
                argument const *arg,
                size_t arg_num)
{
  sqlite3_stmt *prepared = 0;
  sqlite3_prepare_v2(db, stmt.c_str(), stmt.size(), &prepared, NULL);
  for(size_t i=0; i < arg_num; ++i) 
    sqlite3_bind_text(prepared, arg[i].seq_num, arg[i].value, arg[i].size, SQLITE_STATIC);
  int code = SQLITE_DONE;
  while ( SQLITE_DONE != (code = sqlite3_step(prepared)) ) {
    if ( code == SQLITE_BUSY ) continue;
    if ( SQLITE_ROW != code ) break;
    result.push_back(json::object_t());
    json::object_t &obj = mbof(result.back()).object();
    int col_cnt = sqlite3_column_count(prepared);
    for(int i=0; i< col_cnt; ++i) {
      char const *col_name = sqlite3_column_name(prepared, i);
      json::var_t &v = obj[col_name];
      char const *col_i = (char const *)sqlite3_column_text(prepared, i); 
      char const *beg = col_i;
      char const *end = beg + sqlite3_column_bytes(prepared, i);
      if(!json::phrase_parse(beg, end, v) || beg != end)
        v = std::string(col_i);
    }
  }
  if( code != SQLITE_DONE ) {
    sqlite3_finalize(prepared);
    return code;
  }
  return sqlite3_finalize(prepared);
}

} // namespace jsqlite
