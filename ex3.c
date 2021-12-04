#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define LEN 150
#define MAX 100
#define SEUIL 3

int ecrire_fils(int nb, char *name)
{
    FILE *file = fopen(name, "w");
    if (file == NULL)
    {
        return EXIT_FAILURE;
    }
    if (fputc(nb, file) == EOF)
    {
        return EXIT_FAILURE;
    }
    if (fclose(file) == 0)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int lire_pere(int *nb, char *name)
{
    FILE *file = fopen(name, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Le fichier n'a pas pu être ouvert\n");
        return 1;
    }
    else
    {
        printf("Fichier ouvert en lecture\n");
    }
    int n = (int)fgetc(file);
    if (n != EOF)
    {
        *nb = n;
    }
    if (fclose(file) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux\n");
        return 1;
    }
    if (remove(name) == 0)
    {
        printf("Le fichier a été détruit avec succès.\n");
    }
    else
    {
        printf("Impossible de détruire le fichier\n");
        perror("Erreur");
        return 1;
    }
}

int maxi(int i, int j)
{
    if (i > j)
    {
        return i;
    }
    else
    {
        return j;
    }
}

int max(int *tab, int debut, int fin)
{
    int max = tab[debut];
    for (int i = debut; i <= fin; i++)
    {
        max = maxi(max, tab[i]);
    }
    return max;
}

int find_max(int *tab, int debut, int fin)
{
    if ((fin - debut + 1) > SEUIL)
    {
        int status1, status2;
        int max_debut = tab[debut];
        int max_fin = tab[fin];

        printf("Création de deux processus\n");
        pid_t pid1 = fork();
        if (pid1 == 0)
        {
            printf("Ici le fils 1 %d, tableau a evaluer : [%d:%d]\n", getpid(), debut, (debut + fin) / 2);
            printf("Fils 1 %d stop\n", getpid());
            kill(getpid(), SIGSTOP);
            printf("Reprise fils 1 %d\n", getpid());
            int local_max = find_max(tab, debut, (debut + fin) / 2);
            int ctrl = ecrire_fils(local_max, "fichier1");
            if (ctrl == 0)
            {
                printf("Ecriture fichier 1 ok\n");
            }
            exit(EXIT_SUCCESS);
        }
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            printf("Ici le fils 2 %d, tableau a evaluer : [%d:%d]\n", getpid(), ((debut + fin) / 2) + 1, fin);
            int local_max = find_max(tab, ((debut + fin) / 2) + 1, fin);
            int ctrl = ecrire_fils(local_max, "fichier2");
            if (ctrl == 0)
            {
                printf("Ecriture fichier 2 ok\n");
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(pid2, &status1, 0);
            int max_fin;
            lire_pere(&max_fin, "fichier2");
            printf("MAX FIN : %d\n", max_fin);
            printf("Fin fils2 %d\n", pid2);
            printf("fils 1 %d back\n", pid1);
            kill(pid1, SIGCONT);
            waitpid(pid1, &status2, 0);
            printf("Fin fils1 %d\n", pid1);
            int max_debut;
            lire_pere(&max_debut, "fichier1");
            printf("MAX DEBUT : %d\n", max_debut);

            printf("Le max du debut est %d et de fin est %d pour le tableau [%d : %d]\n", max_debut, max_fin, debut, fin);
            return maxi(max_debut, max_fin);
        }
    }
    else
    {
        return max(tab, debut, fin);
    }
}

int random_int()
{
    return (rand() % MAX);
}

int main()
{
    srand(time(NULL));
    int tab[LEN];
    for (int i = 0; i < LEN; i++)
    {
        tab[i] = random_int();
        printf("%d\n", tab[i]);
    }
    printf("\nMaximum global du tableau : %d\n", find_max(tab, 0, LEN - 1));
    return 0;
}