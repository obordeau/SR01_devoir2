#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

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
    if (remove(name) != 0)
    {
        perror("remove file");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    pid_t pid1 = fork();
    char name1[] = "fichier1";
    char name2[] = "fichier2";
    int status;
    if (pid1 == 0)
    {
        printf("Je stop le fils 1 (PID=%d)\n", getpid());
        kill(getpid(), SIGSTOP);
        printf("Je suis le fils 1 (PID=%d)\n", getpid());
        int nb1 = 5;
        printf("    Je vais écrire %d dans le fichier \"%s\"\n", nb1, name1);
        ecrire_fils(nb1, name1);
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        printf("Je suis le fils 2 (PID=%d)\n", getpid());
        int nb2 = 15;
        printf("    Je vais écrire %d dans le fichier \"%s\"\n", nb2, name2);
        ecrire_fils(nb2, name2);
        exit(0);
    }
    else
    {
        int st_fils1, st_fils2;
        if (waitpid(pid2, &st_fils1, 0) == -1)
        {
            perror("waitpid");
        }
        kill(pid1, SIGCONT);
        if (waitpid(pid1, &st_fils2, 0) == -1)
        {
            perror("waitpid");
        }
        printf("Je suis le père (PID=%d)\n", getppid());
        printf("    Je vais récupérer le chiffre écrit par mon fils 1 dans le fichier \"%s\"\n", name1);
        int nb1;
        lire_pere(&nb1, name1);
        printf("    Le fils 1 avait écrit %d dans le fichier\n", nb1);
        printf("    Je vais récupérer le chiffre écrit par mon fils 2 dans le fichier \"%s\"\n", name2);
        int nb2;
        lire_pere(&nb2, name2);
        printf("    Le fils 2 avait écrit %d dans le fichier\n", nb2);
    }
    return 0;
}
