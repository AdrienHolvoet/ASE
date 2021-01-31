## TP3 Bibliothèque de synchronisation

Pour ce TP,on a créé les sémaphores (sem.h et sem.c), le fichier prodcons.c mets en oeuvre ces sémaphores pour offrir une solution au problème classique du producteur consommateur. On remarque bien que grâce au semaphores, le producteur ne va pas ajouter des données sur une file pleine et que le consommateur ne va pas essayer de retirer des données d'une file vide.  
Nous pouvons également effectuer un changement de contexte avant que le tampon ne soit plein afin de permettre au consommateur de consommer.

## MakeFile TP3 

Description du makefile :  
Il suffit ici de lancer la commande make et ensuite l'exécutable ./prodcons
