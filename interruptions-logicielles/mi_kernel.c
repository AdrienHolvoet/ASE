
#include "mi_syscall.h"

int current_process = 0; /*0 ou 1*/

struct tlb_entry_s
{
    unsigned unused : 8;
    unsigned virt_page : 12; /* numero de page! */
    unsigned phys_page : 8;  /* numero de page! */
    unsigned execution : 1;
    unsigned ecriture : 1;
    unsigned lecture : 1;
    unsigned is_active : 1;
};

void tlb_add_entry(struct tlb_entry_s entry)
{
    int val;
     
    printf("Ajout TLB entry: vpage %d ppage %d\n", entry.virt_page, entry.phys_page); /* printf vivement conseillé */
    memcpy(&val, &entry, sizeof(int));
    _out(TLB_ADD_ENTRY, val);
}

int vaddr_to_vpage(unsigned long int vaddr)
{
    return (vaddr - (unsigned long int)virtual_memory) / 4096;
}

int ppage_of_vpage(int vpage, int process)
{
    return vpage * 2 + process + 2;
}

static void switch_to_process0(void)
{
    current_process = 0;
    _out(MMU_CMD, MMU_RESET);
}

static void switch_to_process1(void)
{
    current_process = 1;
    _out(MMU_CMD, MMU_RESET);
}

static void mmuhandler()
{
    printf("tentative d'acces illegal\n");
    unsigned long int x = (unsigned)_in(MMU_FAULT_ADDR_LO);
    unsigned long int y = (unsigned)_in(MMU_FAULT_ADDR_HI);
    unsigned long int adresse_fautive = (y << 32) | x;

    printf("le mmuhandler s'est déclenché pour l'Adresse fautive : %llx \n", adresse_fautive);

    int vpage = vaddr_to_vpage(adresse_fautive);
    if (vpage < N / 2) /*Si c'est bien un access à la page*/
    {
        printf("Cette adresse est autorisé, ajout de l'entrée TLB \n");
        struct tlb_entry_s entry;
        memset(&entry, 0, sizeof(entry));
        entry.execution = entry.lecture = entry.ecriture = entry.is_active = 1;
        entry.virt_page = vpage;
        entry.phys_page = ppage_of_vpage(vpage, current_process); /*vpage vers ppage*/
        tlb_add_entry(entry);
    }
    else
    {
        /*Pas de bol c'est pas dans la plage 0*/
        printf("Cette adresse n'est pas autorisé,on quitte \n");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    if (init_hardware("hardware.ini") == 0)
    {
        printf("pb init hardware\n");
        exit(1);
    }
    printf("l'init a reussi.\n");

    /*TOUT LES HANDLER SONT EXECUTE EN MODE MAITRE QUOI QU'IL ARRIVE*/
    IRQVECTOR[16] = switch_to_process0;
    IRQVECTOR[17] = switch_to_process1; /*on dit que quand ce irq est déclenché et on appelle alors le handler switch_to_process1*/
    IRQVECTOR[MMU_IRQ] = mmuhandler;
    _mask(0x1001); /* 0x1000 == passer en mode user, et 0x0001 == activer les interruptions */

    init();
}