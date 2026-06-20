#pragma once

typedef struct {
  const char* city_code;
  const char* city_name;
  double x;
  double y;
} Sec1StmtContext;

typedef struct {
  int cyclist_code;
  const char* cyclist_name;
  const char* city_code;
} Sec2StmtContext;

typedef struct {
  int cyclist_code;
  const char* city_code;
  int seconds;
} Sec3StmtContext;


void visitSec1Stmt(Sec1StmtContext ctx);
void visitSec2Stmt(Sec2StmtContext ctx);
void visitSec3Stmt(Sec3StmtContext ctx);