## TP5 Mémoire virtuelle et swap disque
 
Ce TP met en place le mécanisme de swap disque qui autorise la manipulation par un processus utilisateur d’un espace d’adressage mémoire de taille supérieure à l’espace mémoire physique disponible sur le matériel. Ici on retrouve 2 pages physiques et 4 pages mémoires virtuelles et donc 4 fichiers de swaps.( On peut en avoir plus il suffit de modifier les constantes `NB_VPAGE` 4 & `NB_PPAGE 2`. On dit que pour le besoin de l'exercice on a 2 pages physiques disponibles. Nous avaons défini deux tables vm_mapping[] et pm_mapping[] qui vont nous permettre respectivement de retrouver pour toute page virtuelle la page physique associée , ou pour toute page physique la page virtuelle associée et de déterminer si la page est en mémoire(cad déja disponible et accessible sur la page physique en fonction de l'adresse virtuelle qui a voulu être accédé) ou doit être rechargée depuis le fichier de swap.  
Les `printf` dans le main() explique bien et mettent en relief le processus de swap mis en place.


## MakeFile TP5

Description du makefile :  
Il suffit ici de lancer la commande make et ensuite l'exécutable ./swap
