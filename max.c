#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define MAX 100
#define SEUIL 3

void ecrire_fils(int nb, char *name)
{
    FILE *file = fopen(name, "w");
    if (file == NULL)
    {
        printf("Le fichier n'a pas pu être ouvert\n");
        exit(1);
    }
    else
    {
        printf("Fichier ouvert avec succès\n");
    }
    if (fputc(nb, file) == EOF)
    {
        printf("Erreur lors de l'écriture d'un caractère\n");
        exit(1);
    }
    if (fclose(file) == 0)
    {
        printf("Fichier fermé\n");
        exit(0);
    }
    if (fclose(file) == EOF)
    {
        printf("Erreur lors de la fermeture du flux\n");
        exit(1);
    }

    exit(0);
}

void lire_pere(int *nb, char *name)
{
    FILE *file = fopen(name, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Le fichier n'a pas pu être ouvert\n");
        exit(1);
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
    if (fclose(file) == 0)
    {
        printf("Fichier fermé\n");
    }
    if (fclose(file) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux\n");
        exit(1);
    }
    if (remove(name) == 0)
    {
        printf("Le fichier a été détruit avec succès.\n");
    }
    else
    {
        printf("Impossible de détruire le fichier\n");
        perror("Erreur");
        exit(1);
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
        int status;
        int max_debut = tab[debut];
        int max_fin = tab[fin];

        printf("On sépare le tableau\n");
        pid_t pid1 = fork();
        if (pid1 == 0)
        {
            printf("Ici le fils 1 %d, tableau a evaluer : [%d:%d]\n", getpid(), debut, (debut + fin) / 2);
            ecrire_fils((int)find_max(tab, debut, (debut + fin) / 2), "fichier1");
            exit(EXIT_SUCCESS);
        }
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            printf("Ici le fils 2 %d, tableau a evaluer : [%d:%d]\n", getpid(), ((debut + fin) / 2) + 1, fin);
            ecrire_fils((int)find_max(tab, ((debut + fin) / 2) + 1, fin), "fichier2");
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            int max_debut;
            lire_pere(&max_debut, "fichier1");
            printf("MAX DEBUT : %d\n", max_debut);
            int max_fin;
            lire_pere(&max_fin, "fichier2");
            printf("MAX FIN : %d\n", max_fin);
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
    int tab[5];
    for (int i = 0; i < 5; i++)
    {
        tab[i] = random_int();
        printf("%d\n", tab[i]);
    }
    printf("%d\n", find_max(tab, 0, 4));
    return 0;
}