#include "jsqlite.hpp"
#include "gen.hpp"

namespace jsqlite {

int select_cb(void* result_arg, int col_num, char ** col_val, char **col_name)
{
  std::vector<json::var_t> &result = *(std::vector<json::var_t>*)(result_arg);
  result.push_back(json::object_t());
  json::object_t &obj = json::get<json::object_t>(result.back());
  for(int i=0;i<col_num;i++) {
    if(col_val[i]) {
      char const *beg = col_val[i];
      char const *end = beg + strlen(col_val[i]);
      if(!json::phrase_parse(beg, end, obj[col_name[i]])) 
        result.pop_back();
    }
  }
  return 0;
}

int select(std::vector<json::var_t> &result, sqlite3* db, std::string const &stmt)
{
  int rc;
  char *error=0;
  rc = sqlite3_exec(db, stmt.c_str(), &select_cb, (void*)&result, &error);
  sqlite3_free(error);
  return rc;
}

std::string lit(json::var_t const &variable)
{  return gen_sql_insertable()(variable); }

std::string lit(json::object_t const &object)
{  return gen_sql_insertable()(object); }


} // namespace jsqlite
