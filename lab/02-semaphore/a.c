#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

void child_critical_section(char *filename);
void parent_critical_section(char *filename);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    sem_t *sem;
    pid_t pid;
    char *filename = argv[1];

    sem = sem_open("SEM", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sen_open");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) // child process
    {
        sem_wait(sem);
        child_critical_section(filename);
        sem_post(sem);
        exit(EXIT_SUCCESS);
    }
    else // parent process
    {
        sem_wait(sem);
        parent_critical_section(filename);
        sem_post(sem);
        exit(EXIT_SUCCESS);
    }

    wait(NULL);

    sem_close(sem);
    sem_unlink("SEM");
}

void child_critical_section(char *filename)
{
    printf("Child is writing\n");

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    write(fd, "547PROC1 MYFILE1\n", strlen("547PROC1 MYFILE1\n"));
    close(fd);
    printf("Child done\n");
}

void parent_critical_section(char *filename)
{
    printf("Parent is writing\n");

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    write(fd, "547PROC2 MYFILE2\n", strlen("547PROC2 MYFILE2\n"));
    close(fd);
    printf("Parent done\n");
}