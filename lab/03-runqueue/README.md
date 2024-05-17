# Runqueue长度观测实验

## 题目

根据给定的ebpf编程框架，填写相关代码，使得该ebpf程序能够观察信创操作系统中CFS调度队列上runqueue的长度。

## 解答

需要观察运行队列的长度，首先找到相关的数据结构在内核中的位置，内核源码`include/linux/sched.h`储存了和调度相关的数据结构，在其中可以找到结构体`sched_entity`，这个结构体当中包含了任务的调度信息，如优先级、运行时间等。在这个结构体当中，可以找到在ebpf文件当中给出的结构体`cfs_rq`，该结构体中记录了CFS队列中运行队列的长度。

以`rqlen.bt`中给出的示例为例：
```c
struct cfs_rq {
	struct load_weight load;
	unsigned int nr_running;
	unsigned int h_nr_running;
};
```
`nr_running`代表运行队列中正在运行的任务数量，因此只需跟踪`nr_running`即可。

在bpftrace中，可以使用`lhist`内置函数打印直方图，便于结果的观察。

补充的代码如下：
```d
profile:hz:99
{
	$task = (struct task_struct *)curtask;
	$rq = (struct cfs_rq *)$task->se.cfs_rq;
	$len = $rq->nr_running;
	$len = $len > 0 ? $len - 1 : 0;
	@runqlen = lhist($len, 0, 10, 1);
}
```
使用`task`变量记录当前任务的相关信息，`rq`记录了当前任务的`sched_entity`结构体中的`cfs_rq`结构体，最后变量`len`记录队列长度，由于`nr_running`会同时记录当前运行的任务和正在等待的任务，因此当任务数量大于0时，要减掉当前正在运行的任务。