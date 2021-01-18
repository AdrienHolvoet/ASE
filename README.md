# TPs ASE

Adrien Holvoet : adrien.holvoet.etu@univ-lille.fr

## TP1 Première manipulation pratique & Bibliothèque try()/throw()
**La première manipulation pratique** se trouve dans le fichier display_stack.c. Dedans se trouve la fonction display_stack permettant d'afficher le RSP/RBP de la pile d'éxécution  
 
**Bibliothèque try()/throw()** se trouve dans les fichier try.c et try.h
Dedans on y retrouve la structure *ctx_s* qui contiendra la copie de nos registres RSP et RBP, la function *try* qui sauvegardera le contexte(cadre de pile) d'elle-même et qui return la fonction passée en paramètre. Dans cette function en paramètre appelé foo(int x) nous retrouverons un appel à la function *throw* qui aura pour but de restaurer le contexte enregistré dans la function *try* et d'empêcher l'éxécution de foo de return directement dans la function main où la function *try* a été appelé.

