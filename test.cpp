#include <iostream>
#include "js_sqlite3/sqlite3.h"
#include "json/accessor.hpp"
#include "jsqlite.hpp"

int main()
{
  using namespace jsqlite;
  using namespace std;

  sqlite3 *db;

  sqlite3_open("person.db", &db);
  sqlite3_enable_load_extension(db, 1);

  json::object_t person;

  person["name"]    = string("DOD'OGIN");
  person["email"]   = string("dog@nuweb.cc");
  person["age"]     = 12u;
  person["connect"] = json::array_t(2);

  json::member_of(person["connect"])[0].value(string("Yang Acer"));
  json::member_of(person["connect"])[1].value(string("Cheng YY"));

  cerr << lit(person) << endl;
  
  cerr << lit(person["connect"]) << endl;

  return 0;
}
