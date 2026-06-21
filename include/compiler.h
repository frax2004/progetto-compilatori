#pragma once
#include "parser.h"

#define Token(T) struct { T value; YYLTYPE where; }

typedef enum {
  LEXICAL_ERROR,
  SYNTAX_ERROR,
  SEMANTIC_ERROR,
} ErrorType;

typedef struct {
  Token(char*) city_code;
  Token(char*) city_name;
  Token(double) x;
  Token(double) y;
} Sec1StmtContext;

typedef struct {
  Token(int) cyclist_code;
  Token(char*) cyclist_name;
  Token(char*) city_code;
} Sec2StmtContext;

typedef struct {
  Token(int) cyclist_code;
  Token(char*) city_code;
  Token(int) seconds;
} Sec3StmtContext;

typedef struct Entry {
  char* city_code;
  char* city_name;
  double x;
  double y;
  struct Entry* next;
} Entry;

typedef struct {
  Entry** entries;
  unsigned long long size;
  unsigned long long capacity;
} HashTable;

typedef struct {
  char* cyclist_name;
  char* city_begin;
  char* city_end;
  double total_time;
  double total_distance;
} Symbol;

typedef unsigned int Key;

typedef struct {
  Symbol symbols[1000];
} SymbolTable;

typedef struct {
  double x;
  double y;
} Vec2;

typedef struct {
  SymbolTable symbol_table;
  HashTable hash_table;
  int frontEndOk;
  const char* inputPath;
} CompilationContext;

CompilationContext* getCompilationContext();

Vec2 toCoords(char* city_name);

Symbol* lookup(SymbolTable* self, Key key);
Symbol* insert(SymbolTable* self, Key key, Symbol symbol);
void destroySymbolTable(SymbolTable* self);

void printHashTable(HashTable table);
Entry* hashTableAt(HashTable* self, char* key);
int hashTableContains(HashTable* self, char* key);
void destroyHashTable(HashTable* table);

void visitSec1Stmt(Sec1StmtContext ctx);
void visitSec2Stmt(Sec2StmtContext ctx);
void visitSec3Stmt(Sec3StmtContext ctx);

void emitError(ErrorType kind, YYLTYPE where, const char* fmt, ...);
void emitNote(YYLTYPE where, const char* fmt, ...);


const char* getSymbolName(int symbol);
int getCurrentToken(const void* ctx);
YYLTYPE* getCurrentTokenLocation(const void* ctx);
int getExpectedTokens(const void* yyctx, int yyarg[], int yyargn);
int reportSyntaxError(const void* yyctx);

void compile(const char* input, const char* output);

