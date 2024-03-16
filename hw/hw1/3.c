/**
 * How many processes will be created after running the program listed below?
 * 
 * int main()
 * {
 *     pid_t pid;
 *     for (int i = 0; i < 2; i++)
 *     {
 *         pid = fork();
 *     }
 * }
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    __pid_t pid;

    for (size_t i = 0; i < 2; i++)
    {
        pid = fork();

        if (i == 1) // must sleep in loop 2 to get correct pid
        {
            sleep(1);
        }

        if (pid < 0) // fork failed
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) // in chlid process
        {
            printf("Loop %zu, pid: %d, child of %d\n", i + 1, getpid(), getppid());
        }
        else // in parent process
        {
            printf("Loop %zu, pid: %d\n", i + 1, getpid());
        }
    }
}