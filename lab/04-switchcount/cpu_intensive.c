#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    sleep(5); // Wait of bpftrace scirpt to run

    while (1)
    {
        // Use a magic number to determin the loop time
        int magicnum = (rand() % 2) ? 114514 : 1919810;

        for (volatile unsigned long long i = 0; i < magicnum; i++)
            ; // Run loop

        printf("Give up CPU and sleep\n");
        sleep(1); // When the loop is done sleep to switch
    }
    return 0;
}