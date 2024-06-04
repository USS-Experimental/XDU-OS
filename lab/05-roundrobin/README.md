## RR调度算法定制实验

## 题目

利用给定的RR调度程序，填写相关代码，对信创操作系统中的RR调度算法进行定制，使其能够完成如下功能：

> 若进程现在的执行顺序是：  【1】【2】【3】  
> 那么当第四个进程转为运行态后，调度依然必须完全保证此顺序，即必须保持为：  【1】【2】【3】【4】  
> 也就是说：若当前执行队列是【2】【3】【1】，那么进程【4】不能按照原始的RR那样，插到【1】之后，而是依然要插在【3】之后。  
即此时runqueue在接纳进程【4】后，runqueue内容变为：【2】【3】【4】【1】，而不是【2】【3】【1】【4】

要求：理解测试程序，并运行测试程序对自己定制后的调度算法进行验证。

## 解答

### 测试程序分析

本实验中，共要运行三个程序，bpftrace跟踪程序、正常任务调度程序，插入任务程序，下面分析这三个程序

#### 插入任务进程

见`src/qjumper.c`，这个进程得到自己的pid后将自己的调度策略为实时循环调度（SCHED_RR），并将优先级参数设置为OUR_PRIO，即10。  接下来调用sub函数，循环打印信息59次后停止。

#### 正常任务调度进程

见`src/main.c`，这个程序将会生成两个子进程共三个进程，每一个进程都和上述的插入进程一样设置，并循环打印信息59次后停止。

#### bpftrace跟踪程序

见`src/trace.bt`，这个脚本将会跟踪`sched_switch`中的内容，在每次调度时，打印出先前进程的执行者，优先级和pid，被调度进程的执行者，优先级和pid共六个参数。

#### 未修改时运行结果

完成调度算法的修改后，将要使用这三个程序进行测试，在使用原RR调度算法时，跟踪程序的可能输出如下：

```bash
     CPUx   (cmd)  #Prio  [PID]          [PID]  #Prio  (cmd)
=============================================================
# 正常任务调度进程启动
CPU0 switch (a.out) #120 [ 4336]  --->   [ 4339] #120 (a.out)
CPU0 switch (a.out) #89  [ 4339]  --->   [ 3854] #120 (kworker/0:3)
CPU0 switch (a.out) #89  [ 4338]  --->   [ 4337] #120 (a.out)
CPU0 switch (a.out) #89  [ 4337]  --->   [    0] #120 (swapper/0)
CPU0 switch (a.out) #89  [ 4339]  --->   [ 4338] #89  (a.out)
CPU0 switch (a.out) #89  [ 4338]  --->   [ 4337] #89  (a.out)
CPU0 switch (a.out) #89  [ 4337]  --->   [ 4339] #89  (a.out)
CPU0 switch (a.out) #89  [ 4339]  --->   [ 4338] #89  (a.out)
CPU0 switch (a.out) #89  [ 4338]  --->   [ 4337] #89  (a.out)
CPU0 switch (a.out) #89  [ 4337]  --->   [ 4339] #89  (a.out)
# 插入任务进程启动
CPU0 switch (a.out) #89  [ 4339]  --->   [ 4406] #89  (b.out)
CPU0 switch (b.out) #89  [ 4406]  --->   [ 4338] #89  (a.out)
CPU0 switch (a.out) #89  [ 4338]  --->   [ 4337] #89  (a.out)
CPU0 switch (a.out) #89  [ 4337]  --->   [ 4339] #89  (a.out)
CPU0 switch (a.out) #89  [ 4339]  --->   [ 4406] #89  (b.out)
CPU0 switch (b.out) #89  [ 4406]  --->   [ 4338] #89  (a.out)
CPU0 switch (a.out) #89  [ 4338]  --->   [ 4337] #89  (a.out)
CPU0 switch (a.out) #89  [ 4337]  --->   [ 3854] #120 (kworker/0:3)
CPU0 switch (a.out) #89  [ 4337]  --->   [ 4339] #89  (a.out)
CPU0 switch (a.out) #89  [ 4339]  --->   [ 4406] #89  (b.out)
...
```

可以发现，a进程也就是正常任务调度进程，生成的三个进程pid分别为4339、4338和4337，并按照这个顺序进行调度。
当b进程也就是插入任务进程启动后，新的进程pid为4406，被插在了4339后面，顺序变为4339 -> **4406** -> 4338 -> 4337 -> 4339。

### 调度算法修改

根据`src/patch.c`，共有三个地方需要进行修改

1. `kernel/sched/sched.h`中`rt_rq`结构
2. `kernel/sched/rt.c`中`init_rt_rq()`函数
3. `kernel/sched/rt.c`中`__enqueue_rt_entity()`函数

想要理解算法调度的任务，就必须从相关的函数和结构入手，本次实验的核心结构为`rt_rq *rt_rq`和`sched_rt_entity *rt_se`。
`rt_rq`为实时运行队列 (real time run queue)，而`rt_se`为一个任务的调度实体，每一个进程都拥有一个`sched_rt_entity`。

#### `rt_rq`结构修改

实验任务要求在调度时维持原有的顺序，若按照上述的程序运行结果，顺序应该从4339 -> 4338 -> 4337 -> 4339变为4339 -> 4338 -> 4337 -> **4406** -> 4339，即将新的进程插入到原队列中最后一个进入队列的进程之后，为此，需要记录这个进程，观察`rt_rq`的新增内容：

```c
	struct sched_rt_entity *last_enq_se;
	int last_enq_prio;
```

通过在`rt_rq`中增加最后一个进入队列进程的`sched_rt_entity`和其调度优先级，就完成了对于这个进程的跟踪。在添加时，只需要将新的进程添加到对应优先级队列中对应进程后即可。

#### `init_rt_rq()`函数修改

完成了这两个变量的添加后，需要将其初始化，这个任务在`init_rt_rq()`函数完成，将两个新增的变量赋值，这里将`rt_se`赋`NULL`，优先值赋-1即可

#### `__enqueue_rt_entity()`函数修改

完成前置工作之后就可以开始修改调度算法，分析原函数

```c
static void __enqueue_rt_entity(struct sched_rt_entity *rt_se, unsigned int flags)
{
	struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
	struct rt_prio_array *array = &rt_rq->active;
	struct rt_rq *group_rq = group_rt_rq(rt_se);
	struct list_head *queue = array->queue + rt_se_prio(rt_se);

	/*
	 * Don't enqueue the group if its throttled, or when empty.
	 * The latter is a consequence of the former when a child group
	 * get throttled and the current group doesn't have any other
	 * active members.
	 */
	if (group_rq && (rt_rq_throttled(group_rq) || !group_rq->rt_nr_running)) {
		if (rt_se->on_list)
			__delist_rt_entity(rt_se, array);
		return;
	}

	if (move_entity(flags)) {
		WARN_ON_ONCE(rt_se->on_list);
		if (flags & ENQUEUE_HEAD)
			list_add(&rt_se->run_list, queue);
		else
			list_add_tail(&rt_se->run_list, queue);

		__set_bit(rt_se_prio(rt_se), array->bitmap);
		rt_se->on_list = 1;
	}
	rt_se->on_rq = 1;

	inc_rt_tasks(rt_se, rt_rq);
}
```

首先添加两个变量便于使用，注意，按照内核的默认编译参数，这两个变量必须在所有操作前声明。

```c
struct sched_rt_entity *last_enq_se = rt_rq->last_enq_se;
int last_enq_prio = rt_rq->last_enq_prio;
```

`last_enq_se`为最后进入队列进程的调度实体，`last_enq_prio`为最后进入队列进程的优先级，后续判断是否需要在最后进入队列进程后插入新的进程需要这两个变量进行判断。

要插入，必须满足最后进入队列实体的`rt_se`不为空。同时`rt_se`中的`on_list`为1，表明这个实体目前正在队列当中。最后还需要判断当前实体的优先级和最后进入队列实体的优先级是否相同，即`(last_enq_se != NULL && last_enq_se->on_list == 1 && last_enq_prio == rt_se_prio(rt_se)) == true`，满足该条件则插入。

原函数在显式的指定插入队列头部时，调用`list_add()`函数，将新的调度实体插入队列头部，而在其他情况下，则调用`list_add_tail()`函数插入队列尾部。

`list_add()`传入两个参数，第一个为需要插入的实体的`list_head`，第二个为插入位置的实体的`list_head`，因此需要插入时，调用`list_add(&rt_se->run_list, &last_enq_se->run_list)`即可完成插入。

修改细节见`src/patch.c`

### 重新编译安装内核

完成修改后，重新编译安装内核，见根目录`README.md`

### 测试

完成调度算法修改后，运行测试程序，可能的结果如下:

```bash
     CPUx   (cmd)  #Prio  [PID]          [PID]  #Prio  (cmd)
=============================================================
# 正常任务调度进程启动
CPU0 switch (a.out) #89  [72512]  --->   [72511] #89  (a.out)
CPU0 switch (a.out) #89  [72511]  --->   [72510] #89  (a.out)
CPU0 switch (a.out) #89  [72510]  --->   [72512] #89  (a.out)
CPU0 switch (a.out) #89  [72512]  --->   [72511] #89  (a.out)
CPU0 switch (a.out) #89  [72511]  --->   [72510] #89  (a.out)
CPU0 switch (a.out) #89  [72510]  --->   [72512] #89  (a.out)
# 插入任务进程启动
CPU0 switch (a.out) #89  [72512]  --->   [72511] #89  (a.out)
CPU0 switch (a.out) #89  [72511]  --->   [72510] #89  (a.out)
CPU0 switch (a.out) #89  [72510]  --->   [72601] #89  (b.out)
CPU0 switch (b.out) #89  [72601]  --->   [72512] #89  (a.out)
CPU0 switch (a.out) #89  [72512]  --->   [72511] #89  (a.out)
CPU0 switch (a.out) #89  [72511]  --->   [72510] #89  (a.out)
CPU0 switch (a.out) #89  [72510]  --->   [72601] #89  (b.out)
CPU0 switch (b.out) #89  [72601]  --->   [21510] #120 (kworker/0:0)
CPU0 switch (b.out) #89  [72601]  --->   [72512] #89  (a.out)
...
```

可以看到，原有的顺序为72512 -> 72511 -> 72510 -> 72512变为72512 -> 72511 -> 72510 -> 72601 -> 72512，插入在正确的位置上，修改成功。