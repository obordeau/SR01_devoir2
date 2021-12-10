#include <stdlib.h>    // exit()...
#include <stdio.h>     // printf()...
#include <unistd.h>    // getpid()...
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait()...
#include <errno.h>     // perror()
#include <time.h>

#define LEN 10
#define MAX 100
#define SEUIL 3

// Fonction utilisée et commentée dans l'exercice 2
void ecrire_fils(int nb, char *name)
{
    FILE *file = fopen(name, "w");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if (fputc(nb, file) == EOF)
    {
        perror("fputc");
        exit(EXIT_FAILURE);
    }
    if (fclose(file) != 0)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

// Fonction utilisée et commentée dans l'exercice 2
void lire_pere(int *nb, char *name)
{
    FILE *file = fopen(name, "r");

    if (!file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    int n = (int)fgetc(file);
    if (n != EOF)
    {
        *nb = n;
    }
    if (fclose(file) == EOF)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    if (remove(name) != 0)
    {
        perror("remove file");
        exit(EXIT_FAILURE);
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

        pid_t pid1 = fork();
        if (pid1 == 0)
        {
            if (kill(getpid(), SIGSTOP) == -1)
            {
                printf("Un fils n'a pas pu être stoppé\n");
                perror("kill");
                return (EXIT_FAILURE);
            }
            int local_max = find_max(tab, debut, (debut + fin) / 2);
            ecrire_fils(local_max, "fichier1");
            exit(EXIT_SUCCESS);
        }
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            int local_max = find_max(tab, ((debut + fin) / 2) + 1, fin);
            ecrire_fils(local_max, "fichier2");
            exit(EXIT_SUCCESS);
        }
        else
        {
            if (waitpid(pid2, &status2, 0) == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            if (status2 != 0)
            {
                printf("Un fils a retourné un statut d'erreur\n");
                exit(EXIT_FAILURE);
            }
            int max_fin;
            lire_pere(&max_fin, "fichier2");

            if (kill(pid1, SIGCONT) == -1)
            {
                printf("Un fils n'a pas pu être réactivé\n");
                perror("kill");
                return (EXIT_FAILURE);
            }
            if (waitpid(pid1, &status1, 0) == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (status1 != 0)
            {
                printf("Un fils a retourné un statut d'erreur\n");
                exit(EXIT_FAILURE);
            }

            int max_debut;
            lire_pere(&max_debut, "fichier1");

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