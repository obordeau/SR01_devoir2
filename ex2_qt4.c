#include <stdlib.h>    // exit()...
#include <stdio.h>     // printf()...
#include <unistd.h>    // getpid()...
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait()...
#include <errno.h>     // perror()

#define NOMFICHIER "fichier"
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
    case -1: // si pid == -1 alors le fork n'a pas fonctionné
        perror("fork");
        return (EXIT_FAILURE);
        break;
    case 0: // si pid == 0 alors on est dans le fils
        printf("Je suis le fils\n");
        printf("    Je vais écrire %d dans le fichier \"%s\"\n", NB, NOMFICHIER);
        ecrire_fils(NB, NOMFICHIER); // on écrit NB dans le fichier de nom NOMFICHIER
        break;
    default:           // sinon pid > 0 alors on est dans le père
        wait(&status); // on attends que son fils retourne un statut
        printf("Je suis le père\n");
        if (status == 0) // si le statut retourné est 0 alors son fils a bien écrit dans le fichier
        {
            printf("    Je vais récupérer le nombre écrit par mon fils dans le fichier \"%s\"\n", NOMFICHIER);
            int nb;
            lire_pere(&nb, NOMFICHIER);                                 // on récupère le nombre écrit dans le fichier par son fils
            printf("    Le fils avait écrit %d dans le fichier\n", nb); // affichage du résultat
        }
        else
        {
            printf("    Le fils n'a pas pu écrire dans son fichier, on arrête le programme principal\n");
            return (EXIT_FAILURE);
        }
        break;
    }
    return 0;
}
