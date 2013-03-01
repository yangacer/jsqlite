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
  
  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS person (name, email TEXT PRIMARY KEY, desc TEXT DEFAULT '\"\"', age, connect);", NULL, NULL, &error);
  PROMPT_ERROR(error);

  sqlite3_exec(db, "SELECT load_extension('js_sqlite3/libjl_sqlite3_regexp.so')", NULL, NULL, &error);
  PROMPT_ERROR(error);

  mbof(person)["name"]    = string("DOD'OGIN");
  mbof(person)["email"]   = string("dog@nuweb.cc");
  mbof(person)["age"]     = boost::intmax_t(12);
  mbof(person)["connect"] = json::array_t(2);

  mbof(person)["connect"](0) = string("Yang Acer");
  mbof(person)["connect"](1) = string("Cheng YY");

  stmt << "INSERT INTO person " << 
    col_names(mbof(person).object()) << " VALUES " << 
    col_values(mbof(person).object())
    ;

  cerr << stmt.str() << "\n";
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

  // ---- transaction ----
  while(1) {
    transaction trans(db, cerr);

    stmt.clear(); stmt.str("");
    stmt << "INSERT INTO person (email, name) VALUES ('\"glory@gmail.com\"', '\"Glory\"');";  // valid insert

    if(sqlite3_exec(db, stmt.str().c_str(), NULL, NULL, &error)) {
      PROMPT_ERROR(error);
      break; // transaction will rollback automatically
    }
    stmt.clear(); stmt.str("");
    stmt << "INSERT INTO person (email, name) VALUES ('\"glory@gmail.com\"', '\"Glory\"');";  // invalid insert

    if(sqlite3_exec(db, stmt.str().c_str(), NULL, NULL, &error)) {
      PROMPT_ERROR(error);
      break; // transaction will rollback automatically
    }
    trans.commit();
    break;
  }
  
  // select_as_map boundary case
  stmt.clear(); stmt.str("");
  stmt << "SELECT * FROM person LIMIT 0;";
  cerr << stmt.str() << "\n";
  json::object_t obj_result;
  jsqlite::select_as_map(0, obj_result, db, stmt.str(), &error);
  PROMPT_ERROR(error);
  assert(obj_result.empty());

  // select_as_map regular case
  stmt.clear(); stmt.str("");
  stmt << "SELECT * FROM person;";

  cerr << stmt.str() << "\n";
  obj_result.clear();
  jsqlite::select_as_map(0, obj_result, db, stmt.str(), &error);
  PROMPT_ERROR(error);
  
  json::pretty_print(cerr, obj_result);
  
  sqlite3_close(db);
  return 0;
}
