/**
 * 2013 Acer Yang <yangacer@gmail.com>
 *
 * Keep my name and add yours.
 */
#include <string.h>
#include <stdio.h>
#include "openssl/sha.h"
#include "sqlite3ext.h"

SQLITE_EXTENSION_INIT1

static void sqlite3_sha1( sqlite3_context *context, int argc, sqlite3_value **argv ) 
{
  if(argc != 1 ) {
    sqlite3_result_null(context);
    return;
  }

  char const *input = (char const*) sqlite3_value_text(argv[0]);
  char output[43];
  char digest[21];
  int i=0;

  if(!SHA1(input, strlen(input), (unsigned char*)digest)) {
    sqlite3_result_null(context);
    return;
  }

  for(i=0;i<20;++i)
    sprintf(&output[i*2+1], "%02x", (unsigned char)digest[i]);

  output[0] = output[41] = '"';
  sqlite3_result_text(context, output, 42, (void*)-1);
}

int sqlite3_extension_init( sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi ) 
{
  SQLITE_EXTENSION_INIT2(pApi)
    int code;
  return sqlite3_create_function(db, "SHA1", 1, SQLITE_ANY, 0, sqlite3_sha1, 0, 0);
}
