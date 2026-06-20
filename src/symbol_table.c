#include "symbol_table.h"

Symbol* lookup(SymbolTable* self, Key key) {
  if(key >= sizeof(self->symbols)/sizeof(*self->symbols)) {
    return NULL;
  }

  return self->symbols[key].cyclist_name != NULL ? 
    self->symbols + key : NULL;
}


Symbol* insert(SymbolTable* self, Key key, Symbol symbol) {
  Symbol* existing = lookup(self, key);

  if(existing != NULL) return NULL;
  
  self->symbols[key] = symbol;
  return self->symbols + key;
}
