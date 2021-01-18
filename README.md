# TP ASE

Adrien Holvoet : adrien.holvoet.etu@univ-lille.fr

## TP1 Première manipulation pratique & Bibliothèque try()/throw()
**La première manipulation pratique** se trouve dans le fichier display_stack.c. Dedans se trouve la fonction display_stack permettant d'afficher le RSP/RBP de la pile d'éxécution  
 
**Bibliothèque try()/throw()** se trouve dans les fichiers try.c et try.h. 
Dedans on y retrouve la structure *ctx_s* qui contiendra la copie de nos registres RSP et RBP, la function *try* qui sauvegardera le contexte(cadre de pile) d'elle-même et qui return la fonction passée en paramètre. Dans cette function passée en paramètre, appelé *foo(int x)*, nous retrouverons un appel à la function *throw* qui aura pour but de restaurer le contexte enregistré dans la function *try* et d'empêcher l'éxécution de foo et de  return directement dans la function main où la function *try* a été appelée.

## MakeFile TP1 

Description des commandes liées makefile :  
- make (all) : crée les exécutables de try et display_stack  
- make display_stack : crée l'exécutable de display_stack   
- make try : crée l'exécutable de try   
- clean : supprime les fichiers temporaires et les produits de la compilation 


