#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "json/json.hpp"
#include "json/accessor.hpp"
#include "js_sqlite3/sqlite3.h"

#define FREE_(x) \
{ \
  sqlite3_free(x); \
  x = 0; \
}

int print_data(void* json_arg, int col_num, char** col_val, char** col_name)
{
  using namespace std;
  using namespace yangacer;
  static unsigned int cnt=0;
  cnt++;
  json::object_t &obj = boost::get<json::object_t>(*(json::var_t*)json_arg);
  
  for(int i=0;i<col_num;i++) {
    if(col_val[i]) {
      char const *beg = col_val[i];
      char const *end = beg + strlen(col_val[i]);
      json::phrase_parse(beg, end, obj[col_name[i]]);
    }
  }
  json::pretty_print(std::cerr, obj);
  return 0;
}

int main()
{
  using namespace std;
  using namespace yangacer;

  // premodel a json object

  json::var_t person = json::object_t();
 

  json::member_of(person)["name"].value(string());
  json::member_of(person)["email"].value(string());
  json::member_of(person)["age"].value(0u);
  json::member_of(person)["connect"].value(json::array_t());

  sqlite3 *db;
  char *error=0;
  
  sqlite3_open("person.db", &db);
  sqlite3_enable_load_extension(db, 1);

  sqlite3_exec(db, "SELECT load_extension('js_sqlite3/libjl_sqlite3_regexp.so')", NULL, NULL, &error);
  if(error) {
    cerr << error << "\n";
    FREE_(error);
  }

  sqlite3_exec(db, "SELECT * FROM person WHERE email REGEXP '.*gmail.com'",
               &print_data, (void*)&person, &error);

  if(error) {
    cerr << error << "\n";
    FREE_(error);
  }
  sqlite3_close(db);

  return 0;
}
