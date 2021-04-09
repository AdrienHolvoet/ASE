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
#define TLB_DEL_ENTRY 0xDE
#define NB_VPAGE 4
#define NB_PPAGE 2
#define MMU_CMD 0x66

static FILE *swap_file = NULL;

static int vm_mapping[NB_VPAGE];
static int pm_mapping[NB_PPAGE];

static int next_page_to_replace = 0;

static int
init_swap(void)
{
	swap_file = fopen(".swap_file", "r+"); /* w+: create, read, write*/
	return swap_file == NULL;
}

int store_to_swap(int vpage, int ppage)
{
	if (swap_file == NULL)
		if (init_swap())
			return -2;
	if (fseek(swap_file, vpage << 12, SEEK_SET) == -1)
		return -1;
	if (fwrite((void *)((ppage << 12) | (uintptr_t)physical_memory),
			   1, PAGE_SIZE, swap_file) == 0)
		return -1;
	return 0;
}

int fetch_from_swap(int vpage, int ppage)
{
	if (swap_file == NULL)
		if (init_swap())
			return -2;
	if (fseek(swap_file, vpage << 12, SEEK_SET) == -1)
	{
		perror("fseek1");
		return -1;
	}
	if (fread((void *)((ppage << 12) | (uintptr_t)physical_memory),
			  1, PAGE_SIZE, swap_file) == 0)
	{
		perror("fread1");
		return -1;
	}
	return 0;
}

struct tlb_entry_s
{
	unsigned unused : 8;
	unsigned virt_page : 12; /* numero de page! */
	unsigned phys_page : 8;	 /* numero de page! */
	unsigned execution : 1;
	unsigned ecriture : 1;
	unsigned lecture : 1;
	unsigned is_active : 1;
};

void tlb_add_entry(struct tlb_entry_s entry)
{
	int val;
	printf("Ajout TLB entry: vpage %d ppage %d\n", entry.virt_page, entry.phys_page);
	memcpy(&val, &entry, sizeof(int));
	_out(TLB_ADD_ENTRY, val);
}

int vaddr_to_vpage(unsigned long int vaddr)
{
	return (vaddr - (unsigned long int)virtual_memory) / 4096;
}

/* On associe la page virtuelle 0 à la page physique 1 */
static void mmuhandler()
{
	printf("\n");
	struct tlb_entry_s entry;
	unsigned long int x = (unsigned)_in(MMU_FAULT_ADDR_LO);
	unsigned long int y = (unsigned)_in(MMU_FAULT_ADDR_HI);
	unsigned long int adresse_fautive = (y << 32) | x;
	printf("Le mmuhandler s'est déclenché pour l'adresse fautive: %lx\n", adresse_fautive);

	int vpage = vaddr_to_vpage(adresse_fautive);

	if (pm_mapping[next_page_to_replace] != -1)
	{
		printf("On enregistre la valeur de la page physique currente %d dans le fichier swap correspondant à la vpage %d\n", next_page_to_replace + 1, pm_mapping[next_page_to_replace]);
		store_to_swap(pm_mapping[next_page_to_replace], next_page_to_replace + 1);
		entry.execution = entry.lecture = entry.ecriture = entry.is_active = 1; /* je met tout à 1 */
		entry.virt_page = pm_mapping[next_page_to_replace];
		entry.phys_page = next_page_to_replace + 1;
		printf("Delete TLB entry: vpage %d ppage %d\n", entry.virt_page, entry.phys_page);
		_out(TLB_DEL_ENTRY, *((int *)(&entry))); /*delete l'entrée tlb */
		vm_mapping[entry.virt_page] = -1;		 /* on remet la valeur -1 à l'entrée du tableau correspondant à la plage virtuelle dont l'entrée tlb vient d'être supprimé, donc elle ne pointe plus vers une adresse physique*/
	}

	printf("On charge le contenu de la page virtuelle %d depuis le swap vers la mémoire physique \n", vpage, next_page_to_replace + 1);
	if (fetch_from_swap(vpage, next_page_to_replace + 1) == -1)
	{
		perror("acces au fichier de swap");
		exit(1);
	}

	vm_mapping[vpage] = next_page_to_replace;
	pm_mapping[next_page_to_replace] = vpage;
	/* rajoute l'entrée de tlb */

	memset(&entry, 0, sizeof(entry));
	entry.execution = entry.lecture = entry.ecriture = entry.is_active = 1; /* je mets tout à 1 */
	entry.virt_page = vpage;
	entry.phys_page = next_page_to_replace + 1;

	next_page_to_replace = (next_page_to_replace + 1) % NB_PPAGE; /*on remplit toute les pages physiques disponibles une fois fait la premiere à remplacer revient à 0*/
	tlb_add_entry(entry);
	printf("\n");
}

int main()
{
	int i;
	for (i = 0; i < NB_PPAGE; i++)
	{
		pm_mapping[i] = -1;
	}
	for (i = 0; i < NB_VPAGE; i++)
	{
		vm_mapping[i] = -1;
	}
	if (init_hardware("hardware.ini") == 0)
	{
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
	*ptr = 0x0;
	printf("la vpage 0 contient: %x\n", *ptr);
	*ptr = 0x10;

	/* maintenant, vpage 1 */
	ptr += 4096; /* avancer d'une page */
	printf("la vpage 1 contient: %x\n", *ptr);
	*ptr = 0x10;
	printf("la vpage 1 contient: %x\n", *ptr);

	ptr += 4096; /* avancer d'une page */
	printf("la vpage 2 contient: %x\n", *ptr);
	*ptr = 0x2;
	printf("la vpage 2 contient: %x\n\n", *ptr);

	ptr += 4096; /* avancer d'une page */
	printf("la vpage 3 contient: %x\n", *ptr);
	*ptr = 0x33;
	printf("la vpage 3 contient: %x\n\n", *ptr);

	ptr -= 4096; /* reculer d'une page */
	printf("Je peux lire sur vpage 2 qui contient: %x\n", *ptr);
	*ptr = 0x0;
	printf("Je peux modifier vpage 2 qui contient sans déclencher mmu: %x\n", *ptr);

	ptr += 4096; /* avancer d'une page */
	printf("Pareil pour la vpage 3 qui  contient: %x\n", *ptr);
	*ptr = 0x44;
	printf("je peux donc la modifier la vpage 3 qui contient maintenant : %x\n", *ptr);

	ptr = virtual_memory; /* retour page 0 */

	printf("J'ai réaccédé à la vpage 0 qui contient: %x ce qui a déclenche le mmu_handler \n", *ptr);
	ptr += 3*4096; /* retour page 0 */

	printf("Je peux toujours accèder la vpage 3 sans déclencher le mmu_handler car c'est la vpage 2 la plus ancienne qu'on a retiré l'entrée tlb, vpage 3 contient: %x\n", *ptr);

	printf("Pages virtuelles accessibles sans déclencher le mmu_handler : ");
	for (i = 0; i < NB_PPAGE; i++)
	{
		printf("%d  ", pm_mapping[i]);
	}
	printf("\n");
	printf("Pages virtuelles n'ayant pas la valeur -1 on une entrée tlb et sont donc accessible sans déclencher le mmu_handler : ");
	for (i = 0; i < NB_VPAGE; i++)
	{
		printf("%d  ", vm_mapping[i]);
	}

	printf("\n");

	exit(1);
}