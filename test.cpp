#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "json/json.hpp"
#include "sqlite3/sqlite3.h"

#define FREE_(x) \
{ \
  sqlite3_free(x); \
  x = 0; \
}

void from_literal(yangacer::json::var_t &v, char const* literal)
{
  using boost::lexical_cast;

  switch(v.which()) {
  case 1:
    v = lexical_cast<unsigned int>(literal);
    break;
  case 4:
    boost::get<std::string>(v).assign(literal);
    break;
  }
};

int print_data(void* json_arg, int col_num, char** col_val, char** col_name)
{
  using namespace std;
  using namespace yangacer;
  json::object_t *json = (json::object_t*)json_arg;

  for(int i=0;i<col_num;i++) {
    from_literal((*json)[col_name[i]], col_val[i]);
  }
  json::pretty_print(std::cerr, *json);
  return 0;
}

int main()
{
  using namespace std;
  using namespace yangacer;

  // premodel a json object

  json::object_t person;

  person["name"] = string();
  person["email"] = string();
  person["age"] = 0u;

  sqlite3 *db;
  char *error=0;
  
  sqlite3_open("person.db", &db);
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS person "
               "(name TEXT, email TEXT PRIMARY KEY, age INTEGER) ",
               NULL, NULL, &error);
  if(error) FREE_(error);

  sqlite3_exec(db, "INSERT INTO person (name, email, age) "
               "VALUES ('Acer Yang', 'yangacer@gmail.com', 30 )",
               NULL, NULL, &error);
  if(error) FREE_(error);

  sqlite3_exec(db, "INSERT INTO person (name, email, age) "
               "VALUES ('Tomato Chou', 'tomatoto@gmail.com', 18 )",
               NULL, NULL, &error);
  if(error) FREE_(error);

  sqlite3_exec(db, "SELECT * FROM person",
               &print_data, (void*)&person, &error);

  if(error) FREE_(error);
  sqlite3_close(db);

  return 0;
}
