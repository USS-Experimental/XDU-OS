/* --------------------------------------------------------------
 * File          : common.h
 * Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
 * Created on    : <2024-02-01>
 * Last modified : <2024-03-01>
 * LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
 * -------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/wait.h>

#define OUR_PRIO 10

void sub(int num)
{
  int tick = 1;
  int duration = 60;

  printf("start %d\n", num);
  time_t start = time(NULL);

  while (1)
    {
      time_t now = time(NULL);
      int delta = now - start;
      if (delta>=duration) break;
      else if (delta>=tick)
        {
          printf("tick [%d], times %d\n", num, tick);
          tick++;
        }
    }
  printf("end %d\n", num);
}
