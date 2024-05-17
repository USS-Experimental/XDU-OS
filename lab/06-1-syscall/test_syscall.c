#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

#define MY_SYSCALL_NUM 451

void test_syscall(int num)
{
    srand(time(NULL) ^ getpid());
    int flag = rand() % 2; // Get random flag

    int result = syscall(MY_SYSCALL_NUM, num, flag); // Call system call
    if (result == -1)
    {
        perror("syscall");
    }
    else
    {
        printf("Process %d: my_syscall(num: %d, flag: %d) = %d\n", getpid(), num, flag, result);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <id> <process number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num = atoi(argv[1]);
    int NUM_PROCESSES = atoi(argv[2]);
    pid_t pids[NUM_PROCESSES];

    for (int i = 0; i < NUM_PROCESSES; i++) // Creat required number of processes
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) // Child process
        {
            test_syscall(num);
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) // Wait for child processes all done
    {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}