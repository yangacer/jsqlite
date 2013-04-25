//#include "utf8_tokenizer.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#ifndef NDEBUG
#include <fstream>
#endif
#include "jl_sqlite3/sqlite3ext.h"
#include "jl_sqlite3/fts3_tokenizer.h"
#include "utf8.h"

#if !defined(SQLITE_CORE) || defined(SQLITE_ENABLE_FTS3)

SQLITE_EXTENSION_INIT1

#ifndef NDEBUG
static std::ofstream log;
#endif

typedef struct utf8_tokenizer {
  sqlite3_tokenizer base;
  char ascii_garbage[128];
} utf8_tokenizer;

typedef struct utf8_tokenizer_cursor {
  sqlite3_tokenizer_cursor base;
  const char *pInput;          /* input we are tokenizing */
  int nBytes;                  /* size of the input */
  int iOffset;                 /* current position in pInput */
  int iToken;                  /* index of next token to be returned */
  char *pToken;                /* storage for current token */
  int nTokenAllocated;         /* space allocated to zToken buffer */
} utf8_tokenizer_cursor;

static int code_size(uint32_t code)
{
  if(code > 0xFFFFu)
    return 4;
  if(code > 0x7FF)
    return 3;
  if(code > 0x7F)
    return 2;
  return 1;
}

inline bool is_garbage(utf8_tokenizer *t, int c)
{ return t->ascii_garbage[c] == 1; }

inline bool is_garbage(uint32_t code)
{ return code > 0xff00u || ( code > 127 && code < 0x3fffu); }

static utf8_tokenizer *utf8_tokenizer_ptr = 0;

extern "C"
utf8_tokenizer* utf8_tokenizer_get_ptr()
{
  return utf8_tokenizer_ptr;
}

/*
** Create a new tokenizer instance.
*/
extern "C" 
int utf8Create(
  int argc, const char * const *argv,
  sqlite3_tokenizer **ppTokenizer
){
  utf8_tokenizer *t ;

  t = (utf8_tokenizer *) sqlite3_malloc(sizeof(*t));
  if( t==NULL ) return SQLITE_NOMEM;
  std::memset(t, 0, sizeof(*t));
#ifndef NDEBUG
  log.open("utf8tok.log", std::ios::out | std::ios::binary);
#endif
  t->ascii_garbage[0] = true;
  for(int i=1; i<0x80; ++i) {
    t->ascii_garbage[i] = (std::iscntrl(i) || std::isspace(i) || std::isblank(i) || std::ispunct(i)) ? 1 : 0;
  }
  *ppTokenizer = &t->base;
  utf8_tokenizer_ptr = t;
  return SQLITE_OK;
}

/*
** Destroy a tokenizer
*/
extern "C" 
int utf8Destroy(sqlite3_tokenizer *pTokenizer){
  sqlite3_free(pTokenizer);
#ifndef NDEBUG
  log.flush();
  log.close();
#endif
  return SQLITE_OK;
}

/*
** Prepare to begin tokenizing a particular string.  The input
** string to be tokenized is pInput[0..nBytes-1].  A cursor
** used to incrementally tokenize this string is returned in 
** *ppCursor.
*/
extern "C" 
int utf8Open(
  sqlite3_tokenizer *pTokenizer,         /* The tokenizer */
  const char *pInput, int nBytes,        /* String to be tokenized */
  sqlite3_tokenizer_cursor **ppCursor    /* OUT: Tokenization cursor */
){
  utf8_tokenizer_cursor *c;
#ifndef NDEBUG
  log << "utf8Open\n";
#endif
  c = (utf8_tokenizer_cursor *) sqlite3_malloc(sizeof(*c));
  if( c==NULL ) return SQLITE_NOMEM;

  c->pInput = pInput;
  if( pInput==0 ){
    c->nBytes = 0;
  }else if( nBytes<0 ){
    c->nBytes = (int)strlen(pInput);
  }else{
    c->nBytes = nBytes;
  }
  c->iOffset = 0;                 /* start tokenizing at the beginning */
  c->iToken = 0;
  c->pToken = NULL;               /* no space allocated, yet. */
  c->nTokenAllocated = 0;

  *ppCursor = &c->base;
  return SQLITE_OK;
}

/*
** Close a tokenization cursor previously opened by a call to
** utf8Open() above.
*/
extern "C" 
int utf8Close(sqlite3_tokenizer_cursor *pCursor){
#ifndef NDEBUG
  log << "utf8Close\n";
#endif
  utf8_tokenizer_cursor *c = (utf8_tokenizer_cursor *) pCursor;
  sqlite3_free(c->pToken);
  sqlite3_free(c);
  return SQLITE_OK;
}

/*
** Extract the next token from a tokenization cursor.  The cursor must
** have been opened by a prior call to utf8Open().
*/
extern "C"
int utf8Next(
  sqlite3_tokenizer_cursor *pCursor,  /* Cursor returned by utf8Open */
  const char **ppToken,               /* OUT: *ppToken is the token text */
  int *pnBytes,                       /* OUT: Number of bytes in token */
  int *piStartOffset,                 /* OUT: Starting offset of token */
  int *piEndOffset,                   /* OUT: Ending offset of token */
  int *piPosition                     /* OUT: Position integer of token */
){
  utf8_tokenizer_cursor *c = (utf8_tokenizer_cursor *) pCursor;
  utf8_tokenizer *t = (utf8_tokenizer *) pCursor->pTokenizer;

  char const
    *beg_it = c->pInput + c->iOffset,
    *end_it = c->pInput + c->nBytes;

  size_t unused = end_it - utf8::find_invalid(beg_it, end_it); // evaluate unused size
  end_it -= unused;
  utf8::unchecked::iterator<char const*> utf8_beg_it(beg_it), utf8_end_it(end_it);
  if(utf8_beg_it.base() < utf8_end_it.base()) {
    // skip garbage
    while( utf8_beg_it != utf8_end_it && ( is_garbage(t, *utf8_beg_it.base()) || is_garbage(*utf8_beg_it))) 
      ++utf8_beg_it;
    c->iOffset += utf8_beg_it.base() - beg_it;
    // non garbage ascii code
    auto token_beg_it = utf8_beg_it;
    while( utf8_beg_it != utf8_end_it && 
           0 <= *utf8_beg_it.base()  && 
           !is_garbage(t, *utf8_beg_it.base())) 
    {
      ++utf8_beg_it;
    }
    size_t n = utf8_beg_it.base() - token_beg_it.base();
    if( n > 0 ) {
      char *new_token;
      new_token = (char*)sqlite3_realloc(c->pToken, n);
      if(!new_token) return SQLITE_NOMEM;
      c->pToken = new_token;
      // normalize to lowercase
      for(int i=0; i<n; i++) {
        unsigned char ch = token_beg_it.base()[i];
        c->pToken[i] = (char)((ch>='A' && ch<='Z') ? ch-'A'+'a' : ch);
      }
      *ppToken = c->pToken;
      *pnBytes = n;
      *piStartOffset = c->iOffset;
      *piEndOffset = c->iOffset + n;
      *piPosition = c->iToken++;
      c->iOffset += n;
#ifndef NDEBUG
      log << "token: ";
      log.write(c->pToken, n);
      log << "\n";
#endif
      return SQLITE_OK;
    } 
    // XXX NULL byte?
    // non garbage unicode
    while( utf8_beg_it != utf8_end_it &&
           *utf8_beg_it.base() < 0 &&
           !is_garbage(*utf8_beg_it))
    {
      // XXX Index *every* unicode word
      char* new_token;
      size_t n = code_size(*utf8_beg_it);
      new_token = (char*)sqlite3_realloc(c->pToken, n);
      if(!new_token) return SQLITE_NOMEM;
      std::memcpy(new_token, utf8_beg_it.base(), n);
      c->pToken = new_token;
      *ppToken = c->pToken;
      *pnBytes = n;
      *piStartOffset = c->iOffset;
      *piEndOffset = c->iOffset + n;
      *piPosition = c->iToken++;
      c->iOffset += n;
#ifndef NDEBUG
      log << "token: ";
      log.write(c->pToken, n);
      log << "\n";
#endif
      return SQLITE_OK;
    }
  }
  return SQLITE_DONE;
}

int registerTokenizer(
    sqlite3 *db, 
    char const *zName, 
    const sqlite3_tokenizer_module *p
){
    int rc;
    sqlite3_stmt *pStmt;
    const char zSql[] = "SELECT fts3_tokenizer(?, ?)";
    
    rc = sqlite3_prepare_v2 (db, zSql, -1, &pStmt, 0);

    if( rc!=SQLITE_OK ){
        return rc;
    }

    sqlite3_bind_text(pStmt, 1, zName, -1, SQLITE_STATIC);
    sqlite3_bind_blob(pStmt, 2, &p, sizeof(p), SQLITE_STATIC);
    sqlite3_step(pStmt);

    return sqlite3_finalize(pStmt);
}

/*
** The set of routines that implement the utf8 tokenizer
*/
extern "C" 
const sqlite3_tokenizer_module utf8TokenizerModule = {
  0,
  utf8Create,
  utf8Destroy,
  utf8Open,
  utf8Close,
  utf8Next,
  0,
};

extern "C" 
int sqlite3_extension_init (
    sqlite3 *db,          /* The database connection */
    char **pzErrMsg,      /* Write error messages here */
    const sqlite3_api_routines *pApi  /* API methods */
) {
    SQLITE_EXTENSION_INIT2(pApi)

    return registerTokenizer(db, "utf8", &utf8TokenizerModule);
}

#endif
