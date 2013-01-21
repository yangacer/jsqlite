#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "json/json.hpp"
#include "json/accessor.hpp"
#include "js_sqlite3/sqlite3.h"
#include "ref_stream.hpp"

struct gen_sql_insert
: boost::static_visitor<std::string>
{
  template<typename T>
  std::string operator()(T const &v) const
  {
    using namespace yangacer;
    std::string result;
    ref_str_stream stream(result);
    json::pretty_print(stream, v, json::print::compact);
    return result;
  }

  std::string operator()(std::string const &s) const
  {
    using namespace yangacer;
    std::string result;
    ref_str_stream stream(result);
    stream << "'";
    json::pretty_print(stream, s, json::print::compact);
    stream << "'";
    return result;
  }

  std::string operator()(yangacer::json::array_t const &a) const
  {
    using namespace yangacer;
    std::string result;
    ref_str_stream stream(result);
    stream << "'";
    json::pretty_print(stream, a, json::print::compact);
    stream << "'";
    return result;
  }

  std::string operator()(yangacer::json::object_t const &o) const
  {
    std::string names, values;
    names = values = "(";

    for(auto i=o.begin();i!=o.end();++i) {
      // form colum names
      if(i != o.begin()) {
        names += ",";
        values += ",";
      }
      names += i->first;
      values += boost::apply_visitor(*this, i->second);
    }
    names += ")";
    values += ")";
    return names + " VALUES " + values;
  }
};

void insert(sqlite3 *db, std::string const &table, yangacer::json::object_t const& json)
{
  gen_sql_insert g;
  char *error=0;

  std::string stmt = "INSERT INTO " + (table + " ");
  stmt += g(json) + ";";

  sqlite3_exec(db, stmt.c_str(), NULL, NULL, &error);
  if(error){
    std::cerr << "error: " << error << "\n";
    sqlite3_free(error);
  }
}

int main()
{
  using namespace std;
  using namespace yangacer::json;

  object_t person;

  person["name"] = string("Cheng \"A\" YY");
  person["email"] = string("chengyy@gmail.com");
  person["age"] = 29u;
  person["connect"] = array_t(3);
  
  member_of(person["connect"])[0].value(string("0001"));
  member_of(person["connect"])[1].value(string("0004"));
  member_of(person["connect"])[2].value(string("0006"));

  sqlite3 *db;
  char *error=0;

  sqlite3_open("person.db", &db);

  insert(db, "person", person);

  person["name"] = string("Yang Acer");
  person["email"] = string("yangacer@gmail.com");
  person["age"] = 30u;
  array_t &connect = get<array_t>(person["connect"]);
  connect.resize(1);
  connect[0] = string("changyy@gmail.com");

  insert(db, "person", person);

  sqlite3_close(db);

  return 0;
}
