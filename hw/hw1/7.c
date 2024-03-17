/**
 * How many "I love OS\n" will be printed in the following program?
 * 
 * int main()
 * {
 *     pid_t pid;
 *     pid = fork();
 *     if (pid == 0)
 *         printf("I love OS\n");
 *     printf("I love OS\n");
 * }
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    __pid_t pid;
    
    pid = fork();

    if (pid == 0)
    {
        printf("pid_t: %d I love OS\n", pid);
    }
    
    printf("pid_t: %d I love OS\n", pid);
}