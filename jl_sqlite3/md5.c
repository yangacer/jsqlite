/**
 * 2013 Acer Yang <yangacer@gmail.com>
 *
 * Keep my name and add yours.
 */
#include <string.h>
#include <stdio.h>
#include "openssl/md5.h"
#include "sqlite3ext.h"

SQLITE_EXTENSION_INIT1

static void sqlite3_md5( sqlite3_context *context, int argc, sqlite3_value **argv ) 
{
  if(argc != 1 ) {
    sqlite3_result_null(context);
    return;
  }

  char const *input = (char const*) sqlite3_value_text(argv[0]);
  char output[35];
  char digest[17];
  int i=0;

  if(!MD5(input, strlen(input), (unsigned char*)digest)) {
    sqlite3_result_null(context);
    return;
  }

  for(i=0;i<16;++i)
    sprintf(&output[i*2+1], "%02x", (unsigned char)digest[i]);

  output[0] = output[33] = '"';
  sqlite3_result_text(context, output, 34, (void*)-1);
}

int sqlite3_extension_init( sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi ) 
{
  SQLITE_EXTENSION_INIT2(pApi)
    int code;
  return sqlite3_create_function(db, "MD5", 1, SQLITE_ANY, 0, sqlite3_md5, 0, 0);
}
