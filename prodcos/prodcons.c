#include "sem.c"
#define N 100

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
        sem_up(&plein);         /* inc. nb place occupees */

        if (plein.val == 50)
        { // test changement de context avant que la tapon soit plein
            yield();
        }
       // print_sem_value(1);
    }
}

void consommateur()
{
    struct objet_t objet;
    while (1)
    {

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

static void timer_it()
{
    printf("TIMER TIK");
    static int tick = 0;
    _out(TIMER_ALARM, 0xFFFFFFFE);
    yield();
}

int main(int argc, char **argv)
{
    unsigned int i;
    /* init hardware */
    if (init_hardware(HARDWARE_INI) == 0)
    {
        fprintf(stderr, "Error in hardware initialization\n");
        exit(EXIT_FAILURE);
    }

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

    /* dummy interrupt handlers */
    for (i = 0; i < 16; i++)
        IRQVECTOR[i] = empty_it;

    /* program timer */
    IRQVECTOR[TIMER_IRQ] = timer_it;      /* j'associe le callback timer_it() a l'evenement 2 (== TIMER_IRQ) */
    _out(TIMER_PARAM, 128 + 64 + 32 + 8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM, 0xFFFFFFFE);        /* alarm at next tick (at 0xFFFFFFFF) */

    /* allows all IT */
    _mask(1);

    /* count for a while... */
    while (1)
    {
    }
    /* and exit! */
    exit(EXIT_SUCCESS);
}