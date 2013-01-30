#include <iostream>
#include <sstream>
#include "jl_sqlite3/sqlite3.h"
#include "json/accessor.hpp"
#include "jsqlite.hpp"

#define PROMPT_ERROR(ec) if(ec){\
  cerr << ec << "\n"; sqlite3_free(ec); ec=0; \
}

int main()
{
  using namespace jsqlite;
  using namespace std;

  sqlite3 *db;
  char *error=0;
  stringstream stmt;
  json::var_t person = json::object_t();
  json::array_t results;

  sqlite3_open("person.db", &db);
  sqlite3_enable_load_extension(db, 1);
  
  sqlite3_exec(db, "SELECT load_extension('js_sqlite3/libjl_sqlite3_regexp.so')", NULL, NULL, &error);
  PROMPT_ERROR(error);

  mbof(person)["name"]    = string("DOD'OGIN");
  mbof(person)["email"]   = string("dog@nuweb.cc");
  mbof(person)["age"]     = 12u;
  mbof(person)["connect"] = json::array_t(2);

  mbof(person)["connect"][0ul] = string("Yang Acer");
  mbof(person)["connect"][1ul] = string("Cheng YY");

  stmt << "INSERT INTO person " << 
    col_names(mbof(person).object()) << " VALUES " << 
    col_values(mbof(person).object())
    ;

  std::cerr << stmt.str() << "\n";

  sqlite3_exec(db, stmt.str().c_str(), NULL, NULL, &error);
  
  PROMPT_ERROR(error);

  stmt.clear();
  stmt.str("");
  
  stmt << "SELECT * FROM person WHERE name == " << 
    lit(mbof(person)["name"].var());

  cerr << stmt.str() << "\n";

  cerr << lit(mbof(person)["connect"].var()) << "\n";

  jsqlite::select(results, db, stmt.str(), &error);

  PROMPT_ERROR(error);

  for( auto i = results.begin(); i != results.end(); ++i)
    json::pretty_print(std::cerr, *i);

  return 0;
}
