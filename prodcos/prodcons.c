#include "sem.c"
#define N 100

/*Le but de la semaphore mutex c'est l'exclusion mutuelle, c'est une sémpahore avec seuleument un jeton donc ça veut dire que on va bloquer l'acces à la ressours si plus
 d'un processeur(thread) essaye d'y accèder, quand un second preccessus essaye d'ya accèder on a le deuxième sem dow qui va faire passer le context en bloqué
 et donc attendra que le 1er thread est fini d'éxécuter la ressource critique pour libérer un jeton avec sem_up( le 2ième thread pourra alors accèder à la ressource critique
 Qui dans ce cas est la mémoire tampon*/

 /*le second type d'utilisation c'est la syncrhonisation de context où la, il faut que le context producteur est fini de remplir la file mémoire tampon pour aisin se bloquer et passer 
 la main à l'autre contexte le consommmateur qui va conssomer l'ensemble des objets dispo dans la mémoire tampon pour ensuite se bloquer et passer la main au contexte producteur.
 ça nous permet de nous assurer que le producteur ne va pas ajouter des données sur une file pleine et que le consommateur ne va pas essayer de retirer des données d'une file vide  */


struct objet_t
{
    char *nom;
    int n;
};

struct tampon_t
{
    struct objet_t objets[N];
    int in;
};

struct tampon_t *tampon;
struct sem_s mutex, vide, plein;

void init_tampon(struct tampon_t *tampon)
{
    tampon->in = 0;
}

void produire_objet(struct objet_t *objet)
{
    objet->nom = "objet produit";
    objet->n = tampon->in;
}

void utiliser_objet(struct objet_t objet)
{
    printf("    %s %d \n ", objet.nom, objet.n);
}

void mettre_objet(struct objet_t objet)
{
    tampon->in++;
    tampon->objets[plein.val] = objet;
}

void retirer_objet(struct objet_t *objet)
{
    *objet = tampon->objets[plein.val];
}

void print_sem_value(int i)
{
    if (i == 0)
    {
        printf(" consommateur  ");
    }
    else
    {
        printf(" producteur  ");
    }
    printf(" plein : %d ", plein.val);
    printf(" mutex : %d ", mutex.val);
    printf(" vide : %d ", vide.val);
}

struct sem_s mutex, vide, plein;

void producteur()
{
    struct objet_t objet;
    while (1)
    {

        produire_objet(&objet); /* produire l’objet suivant */
        sem_down(&vide);        /* dec. nb places libres */
        sem_down(&mutex);       /* entree en section critique */
        mettre_objet(objet);    /* mettre l’objet dans le tampon */
        sem_up(&mutex);         /* sortie de section critique */
        sem_up(&plein);  /* inc. nb place occupees */

        /*  if (plein.val == 50)
        { // test changement de context avant que la tampon soit plein
            yield();
        }*/
        // print_sem_value(1);
    }
}

void consommateur()
{
    struct objet_t objet;
    while (1)
    {
        /*A chaque éxécution, au première tour de boucle : on débloque direct le context producteur et on utilise tout les objets pour ensuite effectuer le yield*/
        sem_down(&plein);      /* dec. nb emplacements occupes */
        sem_down(&mutex);      /* entree section critique */
        retirer_objet(&objet); /* retire un objet du tampon */
        sem_up(&mutex);        /* sortie de la section critique */
        sem_up(&vide);         /* inc. nb emplacements libres */
        print_sem_value(0);
        utiliser_objet(objet); //  utiliser_objet(objet); /* utiliser l’objet */
    }
}

static void count()
{
    while (1)
        ;
}

static void empty_it(void)
{
    return;
}


int main(int argc, char **argv)
{
    unsigned int i;
    /* init hardware */

    tampon = malloc(sizeof(struct tampon_t));
    init_tampon(tampon);
    /* je cree mes contextes */
    create_ctx(0x4000, producteur, NULL);
    create_ctx(0x4000, consommateur, NULL);

    /* je cree mes sémaphores */
    sem_init(&mutex, 1);
    sem_init(&vide, N);
    sem_init(&plein, 0);

    yield();


}
