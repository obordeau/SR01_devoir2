#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid > 0)
    {
        printf("Ici le père %d, au dodo !\n", getpid());
        sleep(10);
    }
    if (pid == 0)
    {
        printf("Ici le processus fils %d, en vie pour 1 minute\n", pid);
    }
    return EXIT_SUCCESS;
}