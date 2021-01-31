#include "sem.h"

struct ctx_s *current_ctx = NULL;
struct ctx_s *ctx_ring = NULL;

void irq_enable()
{
    _mask(1);
}

void irq_disable()
{
    _mask(15);
}

int create_ctx(int stack_size, func_t f, void *args)
{

    struct ctx_s *ctx;

    irq_disable();

    ctx = malloc(sizeof(struct ctx_s));
    if (!ctx_ring)
    {
        ctx->next = ctx;
        ctx_ring = ctx;
    }
    else
    {
        ctx->next = ctx_ring->next;
        ctx_ring->next = ctx;
    }

    ctx->magic = MAGIC;
    ctx->stack = malloc(stack_size);
    ctx->entryPoint = f;
    ctx->etat = READY;
    ctx->copie_rsp = ctx->stack + stack_size - 8;
    ctx->copie_rbp = ctx->copie_rsp;
    ctx->next_blocked = NULL;

    irq_enable();
    return 0;
}

void yield()
{
    if (!current_ctx)
    {

        switch_to_ctx(ctx_ring);
    }
    else
    {
        irq_disable();
        struct ctx_s *target = current_ctx->next;

        struct ctx_s *precedent = current_ctx;
        while (target->etat == TERMINATED || target->etat == WAITING)
        {
            if (target == current_ctx)
            {
                exit(1);
            }

            if (target->etat == TERMINATED)
            {
                /* gerer la suppression de contexte (supprimer le contexte pointe par target */
                if (ctx_ring == target)
                    ctx_ring = target->next;
                /* on est maintenant assuré que ctx_ring ne pointe pas sur target */
                precedent->next = target->next;
                free(target->stack);
                free(target);
            }
            precedent = target;
            target = target->next;
        }
        /* on est sur que target n'est pas TERMINATED */
        irq_enable();
        switch_to_ctx(target);
    }
}

void switch_to_ctx(struct ctx_s *ctx)
{
    irq_disable();
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
    { /* Mikolaï */
        current_ctx->etat = ACTIVABLE;

        irq_enable();
        current_ctx->entryPoint(current_ctx->args);
        /* gerer la terminaison de contexte */
        current_ctx->etat = TERMINATED;
        yield();
    }
    irq_enable();
    return;
}

void sem_init(struct sem_s *sem, unsigned int val)
{
    sem->val = val;
    sem->first_blocked = NULL;
}

void sem_down(struct sem_s *sem)
{
    irq_disable();
    if (sem->val >= 1)
    {
        sem->val -= 1;
        irq_enable();
    }
    else
    {
        current_ctx->etat = WAITING;
        current_ctx->next_blocked = sem->first_blocked;
        sem->first_blocked = current_ctx;
        sem->val -= 1;
        irq_enable();
        yield();
    }
}

void sem_up(struct sem_s *sem)
{
    irq_disable();
    if (sem->val >= 0)
    {
        sem->val += 1;
    }
    else
    {
        if (sem->first_blocked->etat = WAITING)
        {
            /* reveiller un contexte */
            sem->first_blocked->etat = ACTIVABLE;
            sem->first_blocked = sem->first_blocked->next_blocked;
        }
        sem->val += 1;
        irq_enable();
    }
}