#pragma once

typedef enum {
  LEXICAL_ERROR,
  SYNTAX_ERROR,
  SEMANTIC_ERROR,
} ErrorType;

void emitError(ErrorType kind, const char* fmt, ...);