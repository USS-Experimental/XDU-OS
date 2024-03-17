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
        printf("pid: %d pid_t: %d I love OS\n", getpid(), pid);
    }
    
    printf("pid: %d pid_t: %d I love OS\n", getpid(), pid);
}

/**
 * Three "I love OS\n" will be print after execute the upper program.
 * 
 * pid: 5237 pid_t: 5238 I love OS
 * pid: 5238 pid_t: 0 I love OS
 * pid: 5238 pid_t: 0 I love OS
 * 
 * In the parent process, the return value of fork() will be the pid
 * of the child process.
 * In the child process, the return value of fork() will be 0 
*/