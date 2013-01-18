#include <string>
#include <map>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "json/json.hpp"
#include "sqlite3/sqlite3.h"

class schema 
{
public:
  typedef yangacer::json::object_t table_t;
  typedef table_t column_def_t;
  typedef table_t table_schema_t;

  schema(std::string const &dbfile)
  : db_(), zErrMSG_(0)
  {
    if( sqlite3_open(dbfile.c_str(), &db_) )
      throw std::runtime_error("Open/Create db failed");
  }

  ~schema()
  {
    if(zErrMSG_) sqlite3_free(zErrMSG_);
    sqlite3_close(db_);
  }

  void create_table(std::string const& name, table_t const &table)
  {
    auto i = tables_.find(name);
    if (i == tables_.end()) {
      tables_[name] = table;
      std::stringstream stmt;
      stmt << "CRATE TABLE IF NOT EXISTS " << name << "(";
      for ( auto col = table.begin(); col != table.end(); ++col ) {
        stmt << col->first << boost::get<std::string>(col->second) << ",";
      } 
      std::cerr << stmt.str() << "\n";
    }
  }

private:
  sqlite3 *db_;
  char *zErrMSG_;
  table_schema_t tables_;
};

int main()
{
  using namespace std;

  schema s("test.db");
  
  schema::table_t person;
  
  person["email"] = string("TEXT PRIMARY KEY");
  person["name"] = string("TEXT");

  s.create_table("person", person); 

  return 0;
}
