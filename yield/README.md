## TP2 Bibliothèque de création et changement de contexte

Pour ce TP nous avons tout d'abord créé une function *init_ctx* pour créer un contexte ctx avec une pile d’exécution de *stack_size* octets allouée dynamiquement.  
Par la suite, nous avons implémenté la function *switch_to_ctx* pour créer un mécanisme de coroutines(switch d'un contexte à un autre indéfiniement).  
Ensuite nous avons étendu le struct *ctx_s* pour créer une liste circulaire et modifié le *init_ctx* par *create_ctx* pour initialiser dynamiquement la liste chainée de contexte. Et en dernier lieu la primitive *yield()* permettant au contexte courant de passer la main à un autre, tout en gérant l'état TERMINATED, en effet nous skippons le contexte qui passe dans cette état.

## MakeFile TP2

Description du makefile :  
Il suffit ici de lancer la commande make et ensuite l'exécutable ./yield 
