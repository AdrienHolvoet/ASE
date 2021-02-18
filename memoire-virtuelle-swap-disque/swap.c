/* functions return non null value on error */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/hardware.h"

#define MMU_IRQ 13
#define MMU_FAULT_ADDR_LO 0xCC
#define MMU_FAULT_ADDR_HI 0xCD
#define HARDWARE_INI "hardware.ini"
#define PAGE_SIZE 4096
#define TLB_ADD_ENTRY 0xCE
#define MMU_CMD 0x66

static FILE *swap_file = NULL;
static int current_vpage = -1;

static int
init_swap(void) 
{
    swap_file = fopen(".swap_file", "r+"); /* w+: create, read, write*/
    return swap_file == NULL;
}

int
store_to_swap(int vpage, int ppage) 
{
    if (swap_file == NULL)
        if (init_swap()) 
            return -2;
    if (fseek(swap_file, vpage << 12, SEEK_SET) == -1) 
        return -1;
    if (fwrite((void*)((ppage << 12) | (uintptr_t)physical_memory), 
               1, PAGE_SIZE, swap_file) == 0)
        return -1;
  return 0;
}

int 
fetch_from_swap(int vpage, int ppage) 
{
    if (swap_file == NULL)
        if (init_swap()) 
            return -2;
    if (fseek(swap_file, vpage << 12, SEEK_SET) == -1)  {
	perror("fseek1");
        return -1;
    }
    if (fread((void*)((ppage << 12) | (uintptr_t)physical_memory), 
	      1, PAGE_SIZE, swap_file) == 0) {
	perror("fread1");
        return -1;
    }
    return 0;
}


struct tlb_entry_s {
        unsigned unused: 8;
        unsigned virt_page: 12; /* numero de page! */
        unsigned phys_page: 8; /* numero de page! */
        unsigned execution: 1;
        unsigned ecriture: 1;
        unsigned lecture: 1;
        unsigned is_active: 1;
};

void tlb_add_entry(struct tlb_entry_s entry) {
	int val;
	printf("Ajout TLB entry: vpage %d ppage %d\n", entry.virt_page, entry.phys_page); /* printf vivement conseillé */
	memcpy(&val, &entry, sizeof(int));
        _out(TLB_ADD_ENTRY, val);
}

int vaddr_to_vpage(unsigned long int vaddr) {
	return (vaddr - (unsigned long int) virtual_memory) / 4096;
}

/* On associe la page virtuelle 0 à la page physique 1 */ 
static void mmuhandler() {
	unsigned long int x = (unsigned)_in(MMU_FAULT_ADDR_LO);
	unsigned long int y = (unsigned) _in(MMU_FAULT_ADDR_HI);
	unsigned long int adresse_fautive = (y << 32) | x;
	printf("Le mmuhandler s'est déclenché pour l'adresse fautive: %lx\n", adresse_fautive);

	/* est ce que le processus utilisateur a demandé bien acces à la page 0 ? */
	int vpage = vaddr_to_vpage(adresse_fautive);

	/* charger depuis la swap: la page concernée (on utilise qu'une seule page physique pour l'instant: 0 */ 

	/* sauver eventuellement le contenu de la page physique vers le swap */ 
	if (current_vpage != -1)
		printf("On store la page physique %d vers la page virtuelle %d dans le fichier de swap\n", 1, current_vpage);
	if (current_vpage != -1 && store_to_swap(current_vpage, 1) == -1) {
		perror("acces au fichier de swap");
		exit(1);
	}
	printf("On charge la page virtuelle %d depuis le swap vers la memoire physique (page 1)\n", vpage);	
	if(fetch_from_swap(vpage, 1) == -1) {
		perror("acces au fichier de swap");
		exit(1);
	}

	/* rajoute l'entrée de tlb:  vpage <==> ppage:1 */
	struct tlb_entry_s entry;
	memset(&entry, 0, sizeof(entry));
	entry.execution = entry.lecture = entry.ecriture = entry.is_active = 1; /* je met tout à 1 */
	entry.virt_page = vpage;
	entry.phys_page = 1;
	_out(MMU_CMD, MMU_RESET);

	current_vpage = vpage;

	tlb_add_entry(entry); 	

}

int main() {
	if (init_hardware("hardware.ini") == 0) {
		printf("pb init hardware\n");
		exit(1);
	}
	init_swap();
	printf("l'init a reussi.\n");
	printf("physical_memory=%p virtual_memory=%p\n", physical_memory, virtual_memory);

	IRQVECTOR[MMU_IRQ] = mmuhandler;
	_mask(0x1001); /* 0x1000 == passer en mode user, et 0x0001 == activer les interruptions */ 

	char *ptr = virtual_memory;

	/* je lis un truc sur ma vpage 0  */ 
	printf("la vpage 0 contient: %x\n", *ptr);
	*ptr = 0x42;
	printf("la vpage 0 contient: %x\n", *ptr);


	/* maintenant, vpage 1 */ 
	ptr += 4096; /* avancer d'une page */ 
	printf("la vpage 1 contient: %x\n", *ptr);
	*ptr = 0x39;
	printf("la vpage 1 contient: %x\n", *ptr);

	/* je reviens a la vpage 0 */
	ptr = virtual_memory;
	printf("la vpage 0 contient: %x\n", *ptr);
	
	/* maintenant, vpage 1 */ 
	ptr += 4096; /* avancer d'une page */ 
	printf("la vpage 1 contient: %x\n", *ptr);
	exit(1);
}