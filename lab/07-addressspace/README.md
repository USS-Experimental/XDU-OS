# 进程地址空间布局实验

## 题目

根据给定的ebpf编程框架，填写相关代码，使得该ebpf程序能够观察信创操作系统中指定进程的代码段，数据段的起始位置和结束位置，以及栈的起始位置。

## 解答

要观测进程的代码段，就要找到记录相关信息的数据结构，和实验3观察队列长度类似，相关信息也在`include/linux/sched.h`中的`task_struct`记录。  
在`task_struct`中，存在一个结构`mm_struct`，其指向进程的内存描述符，包含代码段、数据段、堆栈等信息。通过跟踪这个结构体的相关信息，即可观察指定进程的代码段，数据段和栈的相关信息。

`mm-struct`中的重要字段如下：
```c
struct mm_struct {
	struct {
        unsigned long start_code, end_code, start_data, end_data;
		unsigned long start_brk, brk, start_stack;
		unsigned long arg_start, arg_end, env_start, env_end;
    }
}
```
其中`start_code`、`end_code`为代码段起始和结束位置，`start_data`、`end_data`为数据起始和结束位置，`start_stack`为栈的起始位置，只需要跟踪这些信息，即可完成任务。

bpftrace脚本如下：
```d
interval:hz:10
/ pid != 0 /
{
	$task = (struct task_struct *)curtask;
	$memory_struct = (struct mm_struct *) $task->mm;

	$start_code = $memory_struct->start_code;
	$end_code = $memory_struct->end_code;
	printf("Code Start: 0x%X, Code End: 0x%X\n", $start_code, $end_code);

	$start_data = $memory_struct->start_data;
	$end_data = $memory_struct->end_data;
	printf("Data Start: 0x%X, Data End: 0x%X\n", $start_data, $end_data);

	$start_stack = $memory_struct->start_stack;
	printf("Stack Start: 0x%X\n", $start_stack);

	printf("\n");
}
```

运行结果如下：
```bash
$ bpftrace mm.bt
Attaching 1 probe...
Code Start: 0x400000, Code End: 0x19731AD
Data Start: 0x1974A90, Data End: 0x19DB1E0
Stack Start: 0xA96041F0

Code Start: 0x400000, Code End: 0x19731AD
Data Start: 0x1974A90, Data End: 0x19DB1E0
Stack Start: 0xA96041F0

Code Start: 0x400000, Code End: 0x19731AD
Data Start: 0x1974A90, Data End: 0x19DB1E0
Stack Start: 0xA96041F0

...
```