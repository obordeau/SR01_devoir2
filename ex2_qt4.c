#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void ecrire_fils(int nb, char *name)
{
    FILE *file = fopen(name, "w");
    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }
    if (fputc(nb, file) == EOF)
    {
        perror("fputc");
        exit(1);
    }
    if (fclose(file) == EOF)
    {
        perror("fclose");
        exit(1);
    }
}

void lire_pere(int *nb, char *name)
{
    FILE *file = fopen(name, "r");

    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }
    int n = fgetc(file);
    if (n != EOF)
    {
        *nb = n;
    }
    if (fclose(file) == EOF)
    {
        perror("fclose");
        exit(1);
    }
    //Ajouté à la question 3
    if (remove(name) != 0)
    {
        perror("remove file");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    pid_t pid = fork();
    char fichier[] = "fichier";
    int status;
    switch (pid)
    {
    case -1:
        printf("Erreur fork\n");
        break;
    case 0:
        printf("Je suis le fils\n");
        int n = 5;
        printf(" Je vais écrire %d dans le fichier \"%s\"\n", n, fichier);
        ecrire_fils(n, fichier);
        break;
    default:
        wait(&status);
        printf("Je suis le père\n");
        printf(" Je vais récupérer le chiffre écrit par mon fils dans le fichier \"%s\"\n", fichier);
        if (status == 0)
        {
            int *nb = (int *)malloc(sizeof(int));
            lire_pere(nb, fichier);
            printf("Le fils avait écrit %d dans le fichier\n", *nb);
        }
        break;
    }
    return 0;
}
