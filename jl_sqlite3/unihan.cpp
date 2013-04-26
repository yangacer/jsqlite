/**
 * 2013 Acer Yang <yangacer@gmail.com>
 *
 * Keep my name and add yours.
 */
#include <cstring>
#include <cstdio>
#include "sqlite3ext.h"
#include "../unihan.hpp"
#include "config.h"

SQLITE_EXTENSION_INIT1

extern "C"
void sqlite3_unihan( sqlite3_context *context, int argc, sqlite3_value **argv )
{
  if(argc != 1 ) {
    sqlite3_result_null(context);
    return;
  }
  static variant vmap(PREFIX"/share/Unihan_Variants_reduce.txt");
  char const *input = (char const*) sqlite3_value_text(argv[0]);
  if(!strlen(input)) {
    sqlite3_result_null(context);
    return;
  }
  size_t ignore;
  std::string output = input;
  output += " OR " + vmap(input, input + strlen(input), ignore);
  sqlite3_result_text(context, output.c_str(), output.size(), reinterpret_cast<void(*)(void*)>(-1));
}

extern "C"
int sqlite3_extension_init( sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi ) 
{
  SQLITE_EXTENSION_INIT2(pApi)
    int code;
  return sqlite3_create_function(db, "unihan", 1, SQLITE_ANY, 0, sqlite3_unihan, 0, 0);
}
