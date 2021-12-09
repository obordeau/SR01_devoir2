#include <stdlib.h>    // exit()...
#include <stdio.h>     // printf()...
#include <unistd.h>    // getpid()...
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait()...
#define FICHIER "fichier"
#define NB 5

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
    pid_t pid = fork(); // on crée un processus fils à l'aide de fork
    int status;
    switch (pid)
    {
    case -1:
        printf("Erreur fork\n");
        break;
    case 0:
        printf("Je suis le fils\n");
        int n = 5;
        printf(" Je vais écrire %d dans le fichier \"%s\"\n", NB, FICHIER);
        ecrire_fils(NB, FICHIER);
        break;
    default:
        wait(&status);
        printf("Je suis le père\n");
        printf(" Je vais récupérer le chiffre écrit par mon fils dans le fichier \"%s\"\n", FICHIER);
        if (status == 0)
        {
            int nb;
            lire_pere(&nb, FICHIER);
            printf("Le fils avait écrit %d dans le fichier\n", nb);
        }
        break;
    }
    return 0;
}
