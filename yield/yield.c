#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "include/hardware.h"
#include "include/hardware_config.h"

#define MAGIC 0xDEADBEEF

typedef void(func_t)(void *);

enum etat_t
{
        READY,
        ACTIVABLE,
        TERMINATED
};

struct ctx_s
{
        void *copie_rsp;
        void *copie_rbp;
        int magic; /* stocke une valeur speciale pour qu'on evite d'utiliser un contexte non initialise */
        void *args;
        char *stack;        /* pointe sur la pile associee a ce contexte */
        func_t *entryPoint; /* pointeur de fonction qui determine le point d'entree du contexte */
        enum etat_t etat;
        struct ctx_s *next;
};

struct ctx_s *current_ctx = NULL;
struct ctx_s *ctxs = NULL;

/* renvoie 0 en cas de succes, autre valeur en cas d'echec (par ex. erreur de malloc)
int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args) { 
    ctx->magic = MAGIC;
    ctx->stack = malloc(stack_size);
    ctx->entryPoint = f;
    ctx->etat = READY;
    ctx->args = args;    
    ctx->copie_rsp = ctx->stack + stack_size - 8;
    ctx->copie_rbp = ctx->copie_rsp;
    
    return 0;
}*/ 


/* renvoie 0 en cas de succes, autre valeur en cas d'echec (par ex. erreur de malloc) */
int create_ctx(int stack_size, func_t f, void *args)
{
        struct ctx_s *new_ctx;

        new_ctx = (struct ctx_s *)malloc(sizeof(struct ctx_s));

        if (ctxs == NULL)
        {
                ctxs = new_ctx;
                ctxs->next = ctxs;
        }
        else
        {
                new_ctx->next = ctxs->next;
                ctxs->next = new_ctx;
        }

        new_ctx->magic = MAGIC;
        new_ctx->entryPoint = f;
        new_ctx->args = args;
        new_ctx->stack = malloc(stack_size);

        assert(new_ctx->stack != NULL);
        new_ctx->copie_rsp = new_ctx->stack + stack_size - 8;
        new_ctx->copie_rbp = new_ctx->copie_rsp;
        new_ctx->etat = READY;
        current_ctx = ctxs;

        return 0;
}



void switch_to_ctx(struct ctx_s *ctx)
{
        if (current_ctx != NULL)
        {
                asm("mov %%rsp, %0"
                    "\n\t"                                                       /* instruction pour copier RSP vers copie_rsp */
                    "mov %%rbp, %1"                                              /* instruction pour copier RBP vers copie_rbp */
                    : "=r"(current_ctx->copie_rsp), "=r"(current_ctx->copie_rbp) /* dans quelles variables on va ecrire */
                    :                                                            /* depuis quelles variables on va lire */
                    :                                                            /* lister l'ensemble des registres qu'on a modifié */
                );
        }

        current_ctx = ctx;

        asm("mov %0, %%rsp"
            "\n\t"                                                     /* instruction pour copier copie_rsp vers RSP */
            "mov %1, %%rbp"                                            /* une instruction pour copier copie_rbp vers RBP */
            :                                                          /* dans quelles variables on va ecrire */
            : "r"(current_ctx->copie_rsp), "r"(current_ctx->copie_rbp) /* depuis quelles variables on va lire */
            :                                                          /* lister l'ensemble des registres qu'on a modifié*/
        );                                                             /*Ici on ne les liste pas à cause de gcc*/

        if (current_ctx->etat == READY)
        {
                current_ctx->etat = ACTIVABLE;

                current_ctx->entryPoint(current_ctx->args);
        }
}

void yield()
{
        struct ctx_s *next = current_ctx->next;
        _out(TIMER_ALARM, 0xFFFFFFFE);
        switch_to_ctx(next);
}

void ping(void *arg)
{
        int i;
        while (1)
        {
                printf("ping! %d\n", i++);
        }
}

void pong(void *arg)
{
        int i;
        while (1)
        {
                printf("pong! %d\n", i++);
        }
}

void pang(void *arg)
{
        int i;
        while (1)
        {
                printf("pang! %d\n", i++);
        }
}

void pung(void *arg)
{
        int i;
        while (1)
        {
                printf("pung! %d\n", i++);
        }
}

static void count()
{
        while (1)
                ;
}

static void empty_it(void)
{
        return;
}

int main()
{
        create_ctx(0x4000, ping, NULL);
        create_ctx(0x4000, pong, NULL);
        create_ctx(0x4000, pang, NULL);
        create_ctx(0x4000, pung, NULL);

        unsigned int i;

        /* init hardware */
        if (init_hardware(HARDWARE_INI) == 0)
        {
                fprintf(stderr, "Error in hardware initialization\n");
                exit(EXIT_FAILURE);
        }

        /* dummy interrupt handlers */
        for (i = 0; i < 16; i++)
                IRQVECTOR[i] = empty_it;

        /* program timer */
        IRQVECTOR[TIMER_IRQ] = yield;
        _out(TIMER_PARAM, 128 + 64 + 32 + 8); /* reset + alarm on + 8 tick / alarm */
        _out(TIMER_ALARM, 0xFFFFFFFE);        /* alarm at next tick (at 0xFFFFFFFF) */

        /* allows all IT */
        _mask(1);

        /* count for a while... */
        count();
        for (i = 0; i < (1 << 28); i++)
                ;

        /* and exit! */
        exit(EXIT_SUCCESS);
}
