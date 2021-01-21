#define MAGIC 0XDEADBEEF
#define EXIT_SUCCES 0
#include <assert.h>
#include<stdio.h>

typedef int (func_t)(int);
struct ctx_s
{
  void *copie_rsp;
  void *copie_rbp;
  int magic;
};
int throw(struct ctx_s * pctx, int r);
int try(struct ctx_s *pctx, func_t *f, int arg);
int foo(int x);