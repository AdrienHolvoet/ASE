#include <stdio.h>
#include <stdint.h>


void display_stack() {
    uint64_t copie_rsp=0, copie_rbp =0;

    asm ("mov %%rsp, %0" "\n\t" 							/* instruction pour copier RSP vers copie_rsp */
       "mov %%rbp, %1" 											/* une instruction pour copier RBP vers copie_rbp */
       : "=r" (copie_rsp), "=r" (copie_rbp) /* dans quelles variables on va ecrire */
       :               											/* depuis quelles variables on va lire */
       :            											/* lister l'ensemble des registres qu'on a modifié */
    );
   printf_s("copie rsp(haut de pile) : %08lx  copie rbp(bas de pile) : %08lx \n", copie_rsp, copie_rbp);
}

int test_addition (int a, int b)
{
	// affichage des adresses des variables a et b 
	printf("adresse de a dans test_addition = %p\n", &a);
	printf("adresse de b dans test_addition = %p\n", &b);
	
	display_stack();
	
	return a+b;
}

int main(void) {
    
    display_stack();

    test_addition(2,5);

    return 0;
}