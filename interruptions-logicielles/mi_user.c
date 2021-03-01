#include "mi_syscall.h"


static int
sum(void *ptr)
{
    int i;
    int sum = 0;

    for (i = 0; i < PAGE_SIZE * N / 2; i++)
        sum += ((char *)ptr)[i];
    return sum;
}
void init(void)
{
    void *ptr = virtual_memory;
    int res;

    _int(SYSCALL_SWTCH_0);                          /*Déclenche l'IRQ 16, ce qui revient à appeler switch_to_proccess0() en mode maitre , si on l'appellait directement on le ferait en mode user donc on pourrait pas reset le MMU
    On déclenche donc l'irq comme cela et quand il est délenché il s'éxécute tjrs en mode maitre
    */
    memset(ptr, 1, PAGE_SIZE * N / 2); /*On remplie nos N/2 pages virtuel avec des 1, ici ça va être les pages pair car on aura switch le current process à 0*/
    _int(SYSCALL_SWTCH_1);
    memset(ptr, 3, PAGE_SIZE * N / 2); /*On remplie nos N/2 pages virtuel avec des 3, ici ça va être les pages impair car on aura switch le current process à 1*/

    _int(SYSCALL_SWTCH_0);
    res = sum(ptr);
    printf("Resultat du processus 0 : %d\n", res);
    _int(SYSCALL_SWTCH_1);
    res = sum(ptr);
    printf("Resultat processus 1 : %d\n", res);
}