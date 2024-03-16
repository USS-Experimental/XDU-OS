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

/**
 * Execute the upper program can get result as followed:
 * 
 * Loop 1, pid: 32324
 * Loop 1, pid: 32325, child of 32324
 * Loop 2, pid: 32325
 * Loop 2, pid: 32324
 * Loop 2, pid: 32327, child of 32325
 * Loop 2, pid: 32326, child of 32324
 * 
 * It can be easily to notice that there are 4 pid: 32324, 32325, 32326 and 32327,
 * which indecate that total 4 process is created.
 * 
 * The process tree is as followed:
 *  32324─┬─32325───32327
 *        └─32326
 * 
 * In loop 1, the parent process(pid: 32324) creates a child process(pid: 32325),
 * in the fork process, all the codes and variables are copyed, 
 * which indate that in child process(pid:32325), the variables i will be 0.
 * 
 * In loop 2, the parent process(pid: 32324) creates a new child process just 
 * like loop 1, whose pid is 32326.
 * The child process(pid: 32325) in loop 1 creates its own child process in loop 2,
 * whose pid is 32327.
 * 
 * After fork process of loop 2, now all four process has the same i, which will be 1,
 * that will end the loop in all four process and that is the end of the program.
 * 
 * One interesting behavior is if there is no sleep in loop 2,
 * the getppid() might get a strange pid(in this test, 413), the print is as followed:
 * 
 * Loop 1, pid: 32324
 * Loop 1, pid: 32325, child of 32324
 * Loop 2, pid: 32325
 * Loop 2, pid: 32324
 * Loop 2, pid: 32327, child of 413
 * Loop 2, pid: 32326, child of 413
 * 
 * With ps -e command, it says that this pid is a process whose CMD is Relay(416).
 * 
 * From my opinion, A possbile explanation of this is that the parent might be terminated
 * before the child process(in this test, 32326 and 32327) call the getppid(), and the
 * system switch the parent of these child process to a process preserved by the system.
*/