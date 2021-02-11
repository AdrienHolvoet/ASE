#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/hardware.h"

#define MMU_IRQ 13
#define MMU_FAULT_ADDR_LO 0xCC
#define MMU_FAULT_ADDR_HI 0xCD
#define TLB_ADD_ENTRY 0xCE
#define N 32 /* N : nombre de page physiques qu'on utilise */
#define PAGE_SIZE 4096

#define MMU_CMD 0x66

#ifndef _MI_H_
#define _MI_H_


#define SYSCALL_SWTCH_0 16
#define SYSCALL_SWTCH_1 17

void init(void);

#endif