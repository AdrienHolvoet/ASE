#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "include/hardware.h"

#define TIMER_PARAM 0xF4
#define TIMER_ALARM 0xF8
#define TIMER_IRQ 2
#define HARDWARE_INI "hardware.ini"
#define MAGIC 0xDEADBEEF

typedef void(func_t)(void *); 

enum etat_t
{
    READY,
    ACTIVABLE,
    TERMINATED,
    WAITING
}; 


struct ctx_s
{
    void *copie_rsp;
    void *copie_rbp;
    int magic;
    void *args;
    char *stack; 

    func_t *entryPoint;

    enum etat_t etat;
    struct ctx_s *next;
    struct ctx_s *next_blocked;
};

int create_ctx(int stack_size, func_t f, void *args);
void switch_to_ctx(struct ctx_s *ctx);
void yield();

struct sem_s
{
    int val;
    struct ctx_s *first_blocked;
};

struct sem_s mon_semaphore;

void irq_enable();
void irq_disable();
void sem_up(struct sem_s *sem);
void sem_down(struct sem_s *sem);
void sem_init(struct sem_s *sem, unsigned int val);



