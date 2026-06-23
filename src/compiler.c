#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "parser.h"
#include "compiler.h"

#define MAX_EDIT_DISTANCE 1

static CompilationContext CONTEXT;

CompilationContext* getCompilationContext() {
  return &CONTEXT;
}

static void construtor() {
  CONTEXT.frontEndOk = 1;
}

static void destructor() {
  CONTEXT.inputPath = NULL;
  destroySymbolTable(&CONTEXT.symbol_table);
  destroyHashTable(&CONTEXT.hash_table);
}


#define CITY_REDECLARATION "Redeclaration of city \"%s\" with city code \"%s\".\n"
#define CYCLIST_REDECLARATION "Redeclaration of cyclist #%d.\n"
#define INVALID_CITY_CODE "No matching city with code \"%s\".%s\n"
#define INVALID_CYCLIST_CODE "No matching cyclist with code #%d.%s\n"
#define IDENTICAL_ADJACENT_CHECKPOINTS "In checkpoint declaration, the next checkpoint (reached by cyclist with code #%d) must be different from the previously reached \"%s\".\n"

void translate(const char* path) {
  if(!CONTEXT.frontEndOk) return;

  FILE* output = fopen(path, "w+");
  if(!output) {
    fprintf(stderr, "Cannot open output file \"%s\"\n", path);
    exit(-1);
  }

  for(int i = 0; i < 1000; i++) {
    Symbol sym = CONTEXT.symbol_table.symbols[i];
    if(lookup(&CONTEXT.symbol_table, i) != NULL) fprintf(
      output,
      "%d, %s <%s, %s> %.5lf\n", 
      i+1, 
      sym.cyclist_name, 
      hashTableAt(&CONTEXT.hash_table, sym.city_begin)->city_name,
      hashTableAt(&CONTEXT.hash_table, sym.city_end)->city_name,
      sym.total_time != 0 ? sym.total_distance/sym.total_time : 0
    );
  }

  fclose(output);
}

void compile(const char* path, const char* output) {
  construtor();

  extern FILE* yyin;
  yyin = fopen(path, "r");

  if(!yyin) {
    fprintf(stderr, "Cannot open file named \"%s\".", path);
    exit(-1);
  }

  CONTEXT.inputPath = path;

  yyparse();
  translate(output);

  destructor();
}


void visitSec1Stmt(Sec1StmtContext ctx) {
  Entry* entry = hashTableAt(&CONTEXT.hash_table, ctx.city_code.value);

  if(entry->city_name != NULL) {
    emitError(SEMANTIC_ERROR, ctx.city_code.where, "%s", ctx.city_code.value);
    emitNote(ctx.city_code.where, CITY_REDECLARATION, entry->city_name, entry->city_code);
    return;
  }

  entry->city_name = ctx.city_name.value;
  entry->x = ctx.x.value;
  entry->y = ctx.y.value;
}

void visitSec2Stmt(Sec2StmtContext ctx) {
  // cyclist_code in [1, 1000] -> key in [0, 999]
  Symbol* symbol = lookup(&CONTEXT.symbol_table, ctx.cyclist_code.value-1);

  int ok = 1;
  if(symbol != NULL) {
    emitError(SEMANTIC_ERROR, ctx.cyclist_code.where, "%d", ctx.cyclist_code.value);
    emitNote(ctx.cyclist_code.where, CYCLIST_REDECLARATION, ctx.cyclist_code.value);
    ok = 0;
  }
  
  if(!hashTableContains(&CONTEXT.hash_table, ctx.city_code.value)) {
    emitError(SEMANTIC_ERROR, ctx.city_code.where, "%s", ctx.city_code.value);
    const char* mostSimilar = mostSimilarCity(ctx.city_code.value, MAX_EDIT_DISTANCE);
    if(mostSimilar != NULL) {
      char buf[64];
      memset(buf, 0, sizeof(buf));
      snprintf(buf, sizeof(buf), " Did you mean city with code \"%s\"?", mostSimilar);
      emitNote(ctx.city_code.where, INVALID_CITY_CODE, ctx.city_code.value, buf);
    } else emitNote(ctx.city_code.where, INVALID_CITY_CODE, ctx.city_code.value, "");

    ok = 0;
  }

  if(ok) {
    // cyclist_code in [1, 1000] -> key in [0, 999]
    insert(&CONTEXT.symbol_table, ctx.cyclist_code.value-1, (Symbol) {
      .cyclist_name = ctx.cyclist_name.value,
      .city_begin = ctx.city_code.value,
      .city_end = ctx.city_code.value,
      .total_distance = 0,
      .total_time = 0
    });
  }
}


Vec2 toCoords(char* city_name) {
  Entry* entry = hashTableAt(&CONTEXT.hash_table, city_name);
  return (Vec2) {
    .x = entry->x,
    .y = entry->y
  };
}



void visitSec3Stmt(Sec3StmtContext ctx) {
  // cyclist_code in [1, 1000] -> key in [0, 999]
  Symbol* symbol = lookup(&CONTEXT.symbol_table, ctx.cyclist_code.value-1);

  int ok = 1;
  if(symbol == NULL) {
    ok = 0;
    emitError(SEMANTIC_ERROR, ctx.cyclist_code.where, "%d", ctx.cyclist_code.value);

    int most_similar = mostSimilarCyclist(ctx.cyclist_code.value, MAX_EDIT_DISTANCE);
    if(most_similar >= 0) {
      char buf[40];
      memset(buf, 0, sizeof(buf));
      snprintf(buf, sizeof(buf), " Did you mean cyclist #%d?", most_similar);
      emitNote(ctx.cyclist_code.where, INVALID_CYCLIST_CODE, ctx.cyclist_code.value, buf);
    }
    else emitNote(ctx.cyclist_code.where, INVALID_CYCLIST_CODE, ctx.cyclist_code.value, "");
  }

  if(!hashTableContains(&CONTEXT.hash_table, ctx.city_code.value)) {
    emitError(SEMANTIC_ERROR, ctx.city_code.where, "%s", ctx.city_code.value);

    const char* mostSimilar = mostSimilarCity(ctx.city_code.value, MAX_EDIT_DISTANCE);
    if(mostSimilar != NULL) {
      char buf[64];
      memset(buf, 0, sizeof(buf));
      snprintf(buf, sizeof(buf), " Did you mean city with code \"%s\"?", mostSimilar);
      emitNote(ctx.city_code.where, INVALID_CITY_CODE, ctx.city_code.value, buf);
    } else emitNote(ctx.city_code.where, INVALID_CITY_CODE, ctx.city_code.value, "");

    ok = 0;
  }

  if(symbol != NULL) {
    char* prev = symbol->city_end;
    char* next = ctx.city_code.value;
  
    if(strcmp(prev, next) == 0) {
      emitError(SEMANTIC_ERROR, ctx.city_code.where, "%s", ctx.city_code.value);
      emitNote(ctx.city_code.where, IDENTICAL_ADJACENT_CHECKPOINTS, ctx.cyclist_code.value, prev);
      ok = 0;
    }
  }

  if(ok) {
    char* prev = symbol->city_end;
    char* next = ctx.city_code.value;

    const Vec2 begin = toCoords(prev);
    const Vec2 end = toCoords(next);

    // cyclist_code in [1, 1000] -> key in [0, 999]
    symbol->city_end = next;
    symbol->total_distance += 1000 * sqrt(pow(end.y - begin.y, 2) + pow(end.x - begin.x, 2));
    symbol->total_time += ctx.seconds.value;
  }
}

int editDistance(const char* s1, const char* s2) {
  #define __EDIST_SUB_COST__(x, y) ((x) != (y))
  #define __EDIST_DEL_COST__(c) (1)
  #define __EDIST_INS_COST__(c) (1)
  #define __EDIST_AT__(i, j) (i)*h+(j)
  #define __EDIST_MIN2__(x, y) (x) < (y) ? (x) : (y)
  #define __EDIST_MIN3__(x, y, z) __EDIST_MIN2__((x), __EDIST_MIN2__((y), (z)))

  int w = strlen(s1)+1;
  int h = strlen(s2)+1;

  int* A = (int*)calloc(w*h, sizeof(int));

  for(int i = 1; i < w; ++i)
    A[__EDIST_AT__(i, 0)] = A[__EDIST_AT__(i-1, 0)] + __EDIST_DEL_COST__(s1[i-1]);

  for(int j = 1; j < h; ++j) {
    A[__EDIST_AT__(0, j)] = A[__EDIST_AT__(0, j-1)] + __EDIST_INS_COST__(s2[j-1]);
  }

  for(int i = 1; i < w; ++i) {
    for(int j = 1; j < h; ++j) {
      A[__EDIST_AT__(i, j)] = __EDIST_MIN3__(
        A[__EDIST_AT__(i-1, j-1)] + __EDIST_SUB_COST__(s1[i-1], s2[j-1]),
        A[__EDIST_AT__(i-1, j)] + __EDIST_DEL_COST__(s1[i-1]),
        A[__EDIST_AT__(i, j-1)] + __EDIST_INS_COST__(s2[j-1])
      );
    }
  }

  int result = A[w*h-1];
  free(A);
  return result;
  
  #undef __EDIST_SUB_COST__
  #undef __EDIST_DEL_COST__
  #undef __EDIST_INS_COST__
  #undef __EDIST_AT__
  #undef __EDIST_MIN2__
  #undef __EDIST_MIN3__
  
}


int mostSimilarCyclist(int code, int threshold) {
  code = code - 1;
  for(int i = 0; i < 1000; i++) {
    if(i == code || lookup(&CONTEXT.symbol_table, i) == NULL) continue;

    char buf1[5];
    char buf2[5];
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
  
    snprintf(buf1, 5, "%d", code);
    snprintf(buf2, 5, "%d", i);
  
    int dist = editDistance(buf1, buf2);
    if(dist <= threshold) return i+1;
  }

  return -1;
}

const char* mostSimilarCity(const char* city, int threshold) {
  
  for(int i = 0; i < CONTEXT.hash_table.capacity; ++i) {
    Entry* list = CONTEXT.hash_table.entries[i];

    while(list != NULL) {
      const char* other = list->city_code;
      if(strcmp(other, city) == 0) continue;
    
      int dist = editDistance(city, other);
      if(dist <= threshold) return other;
      list = list->next;
    }
  }

  return NULL;
}