#include <stdlib.h>    // exit()...
#include <stdio.h>     // printf()...
#include <unistd.h>    // getpid()...
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait()...
#include <errno.h>     // perror()

#define NAME1 "fichier1"
#define NB1 7
#define NAME2 "fichier2"
#define NB2 19

void ecrire_fils(int nb, char *name)
{
    FILE *file = fopen(name, "w"); // ouverture du fichier de nom name
    if (file == NULL)              // ouverture mal déroulée
    {
        perror("fopen"); // erreur d'ouverture
        exit(1);         // sortie fonction
    }
    if (fputc(nb, file) == EOF) // écriture de nb dans le fichier ouvert
    {                           // écriture mal déroulée
        perror("fputc");        // erreur d'écriture
        exit(1);                // sortie fonction
    }
    if (fclose(file) == EOF) // fermeture du fichier
    {                        // fichier mal fermé
        perror("fclose");    // erreur de fermeture
        exit(1);             // sortie fonction
    }
}

void lire_pere(int *nb, char *name)
{
    FILE *file = fopen(name, "r"); // ouverture du fichier de nom name

    if (file == NULL) // ouverture mal déroulée
    {
        perror("fopen"); // erreur d'ouverture
        exit(1);         // sortie fonction
    }
    int n = fgetc(file); // lecture du caractère écrit dans le fichier
    if (n != EOF)        // lecture effectuée sans erreur
    {
        *nb = n; // la valeur pointée par nb prend la valeur récupérée
    }
    if (fclose(file) == EOF) // fermeture du fichier
    {                        // fichier mal fermé
        perror("fclose");    // erreur de fermeture
        exit(1);             // sortie fonction
    }
    //Ajouté à la question 3
    if (remove(name) != 0)     // on détruit le fichier name
    {                          // fichier mal détruit
        perror("remove file"); // erreur de destruction
        exit(1);               // sortie fonction
    }
}

int main(int argc, char const *argv[])
{
    pid_t pid1 = fork(); // on crée un processus fils 1
    int status;
    if (pid1 == -1)
    {
        perror("fork1");
        return (EXIT_FAILURE);
    }
    if (pid1 == 0) // on se situe dans le processus fils 1
    {
        printf("Je stop le fils 1 (PID=%d)\n", getpid());
        if (kill(getpid(), SIGSTOP) == -1) // on stop le fils 1 en attendant que le fils 2 écrive dans son fichier
        {
            printf("Le fils 1 n'a pas pu être stoppé, on arrête le programme principal\n");
            perror("kill");
            return (EXIT_FAILURE);
        }
        printf("Je suis le fils 1 (PID=%d)\n", getpid());
        printf("    Je vais écrire %d dans le fichier \"%s\"\n", NB1, NAME1);
        ecrire_fils(NB1, NAME1); // on écrit NB1 dans NAME1
        exit(0);
    }
    pid_t pid2 = fork(); // on crée un processus fils 2
    if (pid2 == -1)
    {
        perror("fork2");
        return (EXIT_FAILURE);
    }
    if (pid2 == 0) // on se situe dans le processus fils 2
    {
        printf("Je suis le fils 2 (PID=%d)\n", getpid());
        printf("    Je vais écrire %d dans le fichier \"%s\"\n", NB2, NAME2);
        ecrire_fils(NB2, NAME2); // on écrit NB2 dans NAME2
        exit(0);
    }
    else
    {
        int st_fils1, st_fils2;
        if (waitpid(pid2, &st_fils2, 0) == -1) // on attend que le fils 2 renvoie un statut
        {
            perror("waitpid"); // erreur waitpid
        }
        if (st_fils2 == 0) // fils 2 a bien écrit dans son fichier
        {
            printf("On réactive le fils 1\n");
            if (kill(pid1, SIGCONT) == -1) // on réactive le fils 1
            {
                printf("Le fils 1 n'a pas pu être réactivé, on arrête le programme principal\n");
                perror("kill");
                return (EXIT_FAILURE);
            }
        }
        else //
        {
            printf("Le fils 2 n'a pas pu écrire dans son fichier, on arrête le programme principal\n");
            return (EXIT_FAILURE);
        }
        if (waitpid(pid1, &st_fils1, 0) == -1) // on attend que le fils 1 renvoie un statut
        {
            perror("waitpid"); // erreur waitpid
        }
        if (st_fils1 != 0) //le fils 1 a retourné une erreur
        {
            printf("Le fils 1 n'a pas pu écrire dans son fichier, on arrête le programme principal\n");
            return (EXIT_FAILURE);
        }
        printf("Je suis le père (PID=%d)\n", getppid());
        printf("    Je vais récupérer le chiffre écrit par mon fils 1 dans le fichier \"%s\"\n", NAME1);
        int nb1;
        lire_pere(&nb1, NAME1);
        printf("    Le fils 1 avait écrit %d dans le fichier\n", nb1);
        printf("    Je vais récupérer le chiffre écrit par mon fils 2 dans le fichier \"%s\"\n", NAME2);
        int nb2;
        lire_pere(&nb2, NAME2);
        printf("    Le fils 2 avait écrit %d dans le fichier\n", nb2);
    }
    return 0;
}
