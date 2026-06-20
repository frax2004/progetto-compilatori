#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "visitors.h"
#include "hash_table.h"
#include "symbol_table.h"
#include "error.h"
#include <math.h>

static SymbolTable symbol_table = { 0 };
static HashTable hash_table = { 0 };
int frontEndOk = 1;


static void construtor() {
  frontEndOk = 1;
}

static void destructor() {
  destroySymbolTable(&symbol_table);
  destroyHashTable(&hash_table);
}


#define CITY_REDECLARATION "Redeclaration of city %s with city code \"%s\"."
#define CYCLIST_REDECLARATION "Redeclaration of cyclist #%d."
#define INVALID_CITY_CODE "No matching city with code \"%s\"."
#define INVALID_CYCLIST_CODE "No matching cyclist with code #%d."


void translate(const char* path) {
  if(!frontEndOk) return;

  FILE* output = fopen(path, "w+");
  if(!output) {
    fprintf(stderr, "Cannot open output file \"%s\"\n", path);
    exit(-1);
  }

  for(int i = 0; i < 1000; i++) {
    Symbol sym = symbol_table.symbols[i];
    if(lookup(&symbol_table, i) != NULL) fprintf(
      output,
      "%d, %s <%s, %s> %.5lf\n", 
      i+1, 
      sym.cyclist_name, 
      hashTableAt(&hash_table, sym.city_begin)->city_name,
      hashTableAt(&hash_table, sym.city_end)->city_name,
      sym.total_time != 0 ? sym.total_distance/sym.total_time : 0
    );
  }
}

void compile(const char* path, const char* output) {
  construtor();

  extern FILE* yyin;
  yyin = fopen(path, "r");

  if(!yyin) {
    fprintf(stderr, "Cannot open file named \"%s\".", path);
    exit(-1);
  }

  yyparse();

  printHashTable(hash_table);
  
  translate(output);

  destructor();
}


void visitSec1Stmt(Sec1StmtContext ctx) {
  Entry* entry = hashTableAt(&hash_table, ctx.city_code);

  if(entry->city_name != NULL) {
    emitError(SEMANTIC_ERROR, CITY_REDECLARATION, entry->city_name, entry->city_code);
    return;
  }

  entry->city_name = ctx.city_name;
  entry->x = ctx.x;
  entry->y = ctx.y;
}

void visitSec2Stmt(Sec2StmtContext ctx) {
  // cyclist_code in [1, 1000] -> key in [0, 999]
  Symbol* symbol = lookup(&symbol_table, ctx.cyclist_code-1);

  int ok = 1;
  if(symbol != NULL) {
    emitError(SEMANTIC_ERROR, CYCLIST_REDECLARATION, ctx.cyclist_code);
    ok = 0;
  }
  
  if(!hashTableContains(&hash_table, ctx.city_code)) {
    emitError(SEMANTIC_ERROR, INVALID_CITY_CODE, ctx.city_code);
    ok = 0;
  }

  if(ok) {
    // cyclist_code in [1, 1000] -> key in [0, 999]
    insert(&symbol_table, ctx.cyclist_code-1, (Symbol) {
      .cyclist_name = ctx.cyclist_name,
      .city_begin = ctx.city_code,
      .city_end = ctx.city_code,
      .total_distance = 0,
      .total_time = 0
    });
  }
}

typedef struct {
  double x;
  double y;
} Vec2;

Vec2 toCoords(const char* city_name) {
  Entry* entry = hashTableAt(&hash_table, city_name);
  return (Vec2) {
    .x = entry->x,
    .y = entry->y
  };
}


void visitSec3Stmt(Sec3StmtContext ctx) {
  // cyclist_code in [1, 1000] -> key in [0, 999]
  Symbol* symbol = lookup(&symbol_table, ctx.cyclist_code-1);

  int ok = 1;
  if(symbol == NULL) {
    emitError(SEMANTIC_ERROR, INVALID_CYCLIST_CODE, ctx.cyclist_code);
    ok = 0;
  }

  if(!hashTableContains(&hash_table, ctx.city_code)) {
    emitError(SEMANTIC_ERROR, INVALID_CITY_CODE, ctx.city_code);
    ok = 0;
  }

  if(ok) {
    const Vec2 begin = toCoords(symbol->city_end);
    const Vec2 end = toCoords(ctx.city_code);

    // cyclist_code in [1, 1000] -> key in [0, 999]
    symbol->city_end = ctx.city_code;
    symbol->total_distance += 1000 * sqrt(pow(end.y - begin.y, 2) + pow(end.x - begin.x, 2));
    symbol->total_time += ctx.seconds;
  }
}
