/* --------------------------------------------------------------
 * File          : main.c
 * Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
 * Created on    : <2024-02-01>
 * Last modified : <2024-03-01>
 * LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
 * -------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


int sub(pid_t tar_pid, void *tar_addr)
{
    pid_t my_pid = getpid();

    char *str = "I'm Process B. I'll peek other's memory";

    char *my_addr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    strcpy(my_addr, str);

    printf("[B:%d] tar_pid = %d, tar_addr = %p, my_addr = %p\n", my_pid, tar_pid, tar_addr, my_addr);

    printf("[B:%d] Before peek: %s\n",my_pid, my_addr);
    long ret = syscall(548, tar_pid, tar_addr, my_addr);

    if (ret==0)
      {
        printf("[B:%d]  After peek: %s\n\n",my_pid, my_addr);
      }
    else
      {
        printf("Syscall Wrong! ret = %ld\n\n", ret);
      }

    while(1)
      sleep(5);

    return 0;
}


int main(void)
{
    printf("Running....\n");

    pid_t pid = getpid();

    char *str = "Process A's Secret String";

    char *addr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    strcpy(addr, str);

    pid_t sub_pid = fork();
    if (sub_pid==-1)
      {
        printf("Fork Error!\n");
        return 1;
      }
    else if (sub_pid!=0)
      while(1)
      {
	      printf("[A:%d] %p : %s\n\n",pid, addr, addr);
        sleep(5);
      }
    else
      {
        sub(pid, addr);
      }

    return 0;
}

