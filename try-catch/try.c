#include "try.h"


int throw(struct ctx_s * pctx, int r)
{
  static int x;
  x = r;                        // on va sauvegarder l'argument r avant de changer le contexte car les variables locales/arguments/return seront perdu mais pas les variables statics et globales
  assert(pctx->magic == MAGIC); // pour stop la function si throw est pas appelé depuis try

  //asm va modifer le cadre de pile de la function vers le cadre de pile try donc
  //on aura plus acces aux variables de ce cadre de pile courant qui est throw
  //car on aura dépilé donc supprimer le cadre de pile contentant la valeur r
  //pour ça qu'il faut enregistrer la valeur de r dans une variable static avant de bouger les valeurs du cadre.
  asm("mov %0, %%rsp"
      "\n\t"                                       /* instruction pour copier RSP vers copie_rsp */
      "mov %1, %%rbp"                              /* une instruction pour copier RBP vers copie_rbp */
      :                                            /* dans quelles variables on va ecrire */
      : "r"(pctx->copie_rsp), "r"(pctx->copie_rbp) /* depuis quelles variables on va lire */
      :);
  return x;
}

int try(struct ctx_s *pctx, func_t *f, int arg)
{
  pctx->magic = MAGIC;
  /* sauvegarder les adresses du cadre de pile actuel */
  asm("mov %%rsp, %0"
      "\n\t"                                         /* instruction pour copier RSP vers copie_rsp */
      "mov %%rbp, %1"                                /* une instruction pour copier RBP vers copie_rbp */
      : "=r"(pctx->copie_rsp), "=r"(pctx->copie_rbp) /* dans quelles variables on va ecrire */
      :                                              /* depuis quelles variables on va lire */
      :                                              /* lister l'ensemble des registres qu'on a modifié */
  );
  return f(arg);
}

struct ctx_s monContexte;

int foo(int x)
{
  printf("foo est appele avec: %d\n", x);
  throw(&monContexte, 123);   /* va changer la cadre de pile pour revenir au cadre de pile de try donc on a returnera pas ici mais directement dans main */
  printf("apres le throw\n"); /* jamais executé */
  return x + 10;
}

int main(void)
{
  int val = try(&monContexte, foo, 42);
  printf("%d\n", val);

  return EXIT_SUCCES;
}