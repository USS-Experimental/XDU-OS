# 进程切换次数观测实验

## 题目

根据给定的ebpf编程框架，填写相关代码，使得该ebpf程序能够观察信创操作系统中指定进程被调度的次数

## 解答

CFS(Completely Fair Scheduler)是Linux内核中的默认调度器，用于管理和调度进程。相关的实现代码在`kernel/sched/fair.c`下。

### bpftrace脚本实现

根据实验所给的`switchcnt.bt`脚本，需要跟踪`dequeue_task_fair`函数的调用情况。 

在 CFS 中，当一个进程从就绪队列中被选中执行时，会调用`dequeue_task_fair`函数将其从队列中移除，因此，可以通过跟踪该函数的调用情况间接确定该进程被调度的次数。

脚本主体部分如下：
```d
BEGIN
{
  @target_pid = (uint64)$1; // Get the PID from bash script
  @count = 0;
  printf("Count PID: %d, press Ctrl-C to show result\n", @target_pid);
  @time = nsecs
}

kprobe:dequeue_task_fair
/pid == @target_pid/ // Ensure tracking the required process
{
  @count++;
  printf("Process %d, total count: %d\n", @target_pid, @count);
}

END
{
  @time = (nsecs-@time) / 1000000000; // ns to second
}
```
脚本将会接收一个命令行参数输入，该参数为需要跟踪的进程PID。

使用`kprobe:dequeue_task_fair`记录函数调用，注意这里使用了筛选器(Filtering)，只记录对应进程的调用情况。  
当进程每次被调度时，探针都会记录`dequeue_task_fair`函数被调用，`count`加1，直到脚本终止运行，这样就可以间接记录进程被调度的情况。

### 设计进程共脚本跟踪

完成了bpftrace脚本，可以直接通过ps指令查看正在运行的进程并跟踪，完成实验。   
但是为了更好的观察其实现效果，设计一个专用的进程供脚本跟踪是有必要的，根据调度相关的知识，在进程调用系统调用，例如`sleep`时，会主动放弃CPU，操作系统执行调度，因此，可以设计一个阻塞的程序，在特定时刻让其睡眠，观察脚本是否能检测到调度。

程序如下：
```c
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
```
在这个程序当中，等待bpftrace脚本开始运行后进入while死循环，循环中随机选择for循环的次数，完成循环后打印信息，并睡眠放弃CPU。

### 上层脚本实现

完成了bpftrace脚本和实验进程设计后，需要完成一个上层脚本，生成进程供脚本跟踪，并向脚本传递相关的参数，脚本如下：

```bash
#!/bin/bash

# Compile the CPU intensive program
gcc cpu_intensive.c -o cpu_intensive.out

# Run a CPU intensive task and get it's PID
echo "Created a CPU intensive task"
./cpu_intensive.out &
TARGET_PID=$!

# Output the PID of CPU intensive task
echo "CPU intensive task PID: $TARGET_PID"

# Run bpftrace script
bpftrace switchcnt.bt $TARGET_PID

# Kill the CPU intensive task
kill $TARGET_PID
```

脚本编译并运行设计好的一个CPU密集型任务，同时记录该进程的PID，`$!`为后台运行的最后一个进程的ID号，在脚本的运行场景下为新生成并运行的c程序的ID号，将这个ID传给bpftrace脚本，当手动终止bpftrace脚本后，将生成的c程序进程终止。

**注意：该脚本在root用户下运行**

最终的运行结果可能如下：
```bash
$ bash run.sh
Created a CPU intensive task
CPU intensive task PID: 46470
Attaching 3 probes...
Count PID: 46470, press Ctrl-C to show result
Give up CPU and sleep
Process 46470, total count: 1
Give up CPU and sleep
Process 46470, total count: 2
Give up CPU and sleep
Process 46470, total count: 3
Give up CPU and sleep
Process 46470, total count: 4
Give up CPU and sleep
Process 46470, total count: 5
^C

@count: 5
@target_pid: 46470
@time: 6
```
脚本共运行6秒，记录到5次调度。