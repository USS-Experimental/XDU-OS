/* --------------------------------------------------------------
 * File          : main.c
 * Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
 * Created on    : <2024-02-01>
 * Last modified : <2024-03-01>
 * LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
 * -------------------------------------------------------------*/

#include "common.h"

int main(int argc, char const *argv[])
{
  int number_of_subprocess = 3;

  for (int i=1; i<=number_of_subprocess; i++)
    {
      pid_t pid = fork();
      if (pid==-1)
        {
          printf("Fork Error!\n");
          return 1;
        }
      else if (pid!=0)
        continue;
      else
        {
          struct sched_param param = { .sched_priority = OUR_PRIO };
          int retval = sched_setscheduler(pid, SCHED_RR, &param);

          if (retval==-1)
            {
              printf("Setscheduler Error! Maybe you need sudo.\n");
              return 2;
            }
          sub(i);
          return 0;
        }
    }


  int status; pid_t pid;
  while (pid=wait(&status))
    {
      if (pid==-1) break;
      printf("Wait %d\n", pid);
    }
  printf("Final end\n");
  return 0;
}
