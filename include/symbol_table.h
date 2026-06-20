#pragma once

#include <stddef.h>

typedef struct {
  const char* cyclist_name;
  const char* city_begin;
  const char* city_end;
  double total_time;
  double total_distance;
} Symbol;

typedef unsigned int Key;

typedef struct {
  Symbol symbols[1000];
} SymbolTable;


Symbol* lookup(SymbolTable* self, Key key);
Symbol* insert(SymbolTable* self, Key key, Symbol symbol);
