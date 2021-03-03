## TP4 Interruptions logicielles

Le but de ce tp est de résumer l'ensemble des choses vues en tp pour le moment sur la virtualisation de la mémoire du microprocesseur. Pour le moment on retrouve l'unité de virtualisation de la mémoire, la protection de la mémoire et l'isolation de la mémoire pratique. Le code du tp est séparé en deux fichiers sources mi_kernel.c et mi_user.c, ceux- ci correspondent aux  fonctions et séquences de code exécutées en mode maître et utilisateur. Ces deux fichiers se partageront un fichier d’entête mi_syscall.h qui identifie les «appels systèmes» possibles 16 et 17 avec une function *void init(void);* qui donnera la main au code utilisateur. Le but de ce code est de passer alternativement à l'execution du processus 0 et 1. Pour ce faire, nous utilisons les _int(16) et _int(17) permettant ainsi de déclencher un handler en mode maitre afin de switch de processus courant tout en resetant la mémoire(obligatoirement en mode maitre) pour que le switch de processus se fasse correctement. On a un 1er appel comme cela : 
```
_int(16);
memset(ptr, 1, PAGE_SIZE * N/2);
```
Celui-ci permet de reset le MMU et de passer au processus 0 et ainsi lorsque nous appelons memset, le mmu_handler se déclenche on remplit nos N/2 pages virtuel paires avec des 1

juste après nous retrouvons cet appel  
```
_int(17);
memset(ptr, 3, PAGE_SIZE * N/2);
```
Inversement celui-ci permet de reset le MMU et de passer au processus 1 et ainsi lorsque nous appelons memset, le mmu_handler se déclenche et on remplit nos N/2 pages virtuel impaires avec des 3

Ensuite on rappelle à nouveau `_int(16)`(resp.`_int(17)`) pour revenir sur le processus 0(resp.1) et on utilise la fonction `static int sum(void *ptr)` permettant d'additionner l'ensemble des valeurs contenues dans nos plages virtuelles(=pages physiques associées) qui sont des 1(resp.3)

## MakeFile TP4 

Description du makefile :  
Il suffit ici de lancer la commande make et ensuite l'exécutable ./mi_kernel
