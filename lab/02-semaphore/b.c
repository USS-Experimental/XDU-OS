#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

void child_critical_section(char *filename);
void parent_critical_section(char *filename);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

    int shmfd = shm_open("sharedmem", O_CREAT | O_RDWR, 0666);
    if (shmfd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shmfd, sizeof(int)) == -1)
    {
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    int *turn = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (turn == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    *turn = 0;

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) // child process
    {
        while (*turn != 0)
        {
        }
        child_critical_section(filename);
        *turn = 1;
        exit(EXIT_SUCCESS);
    }
    else // parent process
    {
        while (*turn != 1)
        {
        }
        parent_critical_section(filename);
        *turn = 0;
        wait(NULL);
        munmap(turn, sizeof(int));
        close(shmfd);
        shm_unlink("sharedmem");
        exit(EXIT_SUCCESS);
    }

    return 0;
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