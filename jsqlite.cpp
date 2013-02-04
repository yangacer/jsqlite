#include "jsqlite.hpp"
#include "gen.hpp"
#include "boost/variant/apply_visitor.hpp"
#include "json/accessor.hpp"

namespace jsqlite {

typedef int (*exec_cb_type)(void*,int,char**,char**);

inline int 
wait_if_busy(sqlite3 *db, char const *stmt, exec_cb_type cb, void* cb_arg, char** err_str)
{
  int ec=0;
  while ( SQLITE_BUSY == (ec = sqlite3_exec(db,stmt,cb,cb_arg,err_str)) );
  return ec;
}

// ---- transaction impl ----

#define LOG_TRANS_(STMT) \
{ \
  char *err = 0; \
  if(0 != (ec_ = wait_if_busy(db_, STMT, NULL, NULL, &err))) { \
    logger_ << err << "\n"; \
    sqlite3_free(err);  \
    finalized_ = true; \
  } \
}

transaction::transaction(sqlite3 *db, std::ostream &logger)
: db_(db), finalized_(false), logger_(logger), ec_(0)
{
  LOG_TRANS_("BEGIN TRANSACTION;");
}

transaction::~transaction()
{  rollback(); }

bool transaction::commit()
{
  if(!finalized_) {
    LOG_TRANS_("END TRANSACTION;");
    finalized_ = true;
  }
  return ec_ == 0;
}

bool transaction::rollback()
{
  if(!finalized_) {
    LOG_TRANS_("ROLLBACK TRANSACTION;");
    finalized_ = true;
  }
  return ec_ == 0;
}

// ---- select impl ----

int select_cb(void* result_arg, int col_num, char ** col_val, char **col_name)
{
  json::array_t &result = *(json::array_t*)(result_arg);
  result.push_back(json::object_t());
  json::object_t &obj = mbof(result.back()).object();
  for(int i=0;i<col_num;i++) {
    if(col_val[i]) {
      char const *beg = col_val[i];
      char const *end = beg + strlen(col_val[i]);
      if(!json::phrase_parse(beg, end, obj[col_name[i]])) 
        result.pop_back();
    } else {
      obj[col_name[i]];
    }
  }
  return 0;
}

int select(json::array_t &result, sqlite3* db, std::string const &stmt, char **error)
{
  int rc;
  rc = wait_if_busy(db, stmt.c_str(), &select_cb, (void*)&result, error);
  return rc;
}

// --- lit, col_names, col_values, and vector impl ----

std::string lit(json::var_t const &variable)
{  return boost::apply_visitor(gen_sql_insertable(),variable); }

std::string col_names(json::object_t const &object)
{ 
  std::string result = "(";
  for(auto i = object.begin(); i != object.end(); ++i ) {
    if( i != object.begin() )
      result += ",";
    result += i->first;
  }
  result += ")";
  return result;
}

std::string col_values(json::object_t const &object)
{
  std::string result = "(";
  for(auto i = object.begin(); i != object.end(); ++i ) {
    if( i != object.begin() )
      result += ",";
    result += lit(i->second);
  }
  result += ")";
  return result;
}

std::string vector(json::array_t const &array)
{
  std::string result = "(";
  for(auto i = array.begin(); i != array.end(); ++i ) {
    if( i != array.begin() )
      result += ",";
    result += lit(*i);
  }
  result += ")";
  return result;
}

std::string vector_cols(json::array_t const &array)
{
  std::string result = "(";
  for(auto i = array.begin(); i != array.end(); ++i ) {
    if( i != array.begin() )
      result += ",";
    result += cmbof(*i).string();
  }
  result += ")";
  return result;
}

} // namespace jsqlite
