# 进程页表修改实验

## 题目

### 要求

根据给定的系统调用框架，填写相关代码，完成peep_page系统调用，编写a、b两个进程，使得b进程能够窥探a进程中的一个页面信息。

要求：理解测试程序，并运行测试程序对自己的peep_page进行验证。

### 测试

首先运行测试程序，编译并运行 `main.c`，此时应该会得到错误的结果

### 修改内核

请按照 `patch.c` 的内容修改内核，填好TODO的代码。编译并安装好内核，重启，使用该新内核再次进行测试观察。

> 请观测至少10次，才能一定程度上验证你所做的内核修改是正确无误的

## 解答

### 测试程序分析

从`main.c`中可以发现，程序将会创建一个子进程，子进程对应上述题目中的进程B，而父进程则为进程A。

同时，程序中有一个字符串，这个字符串将会被复制到一个匿名的共享内存中，后续需要窥探的就是这一块内存，该内存大小为4KB，恰好为一个页。

父进程功能无需赘述。

子进程调用`sub`函数，传入了父进程的pid和需要窥探的内存地址。

`sub`函数传入的pid为`tar_pid`，传入的地址为`tar_addr`,并得到自己的pid`my_pid`。

之后创建一个字符串，并将其复制到一个匿名的共享内存`my_addr`中，并将本进程的信息和该字符串(调用`my_addr`访问)打印。

完成后，调用需要自行创建的系统调用，窥探A进程的页。

如果成功窥探，B进程将可以打印A进程中的字符串，示例如下：

```
[B:8376] Before peek: I'm Process B. I'll peek other's memory
[B:8376]  After peek: Process A's Secret String
```

如果系统调用失败，则打印如下信息：

```
[B:9568] Before peek: I'm Process B. I'll peek other's memory
Syscall Wrong! ret = -1
```

### 思路

从分析测试程序中可以得知，要使用一个系统调用完成B进程对A进程内的一个页的访问。

假设A进程中的字符串保存在虚拟页vx中，其对应的物理页为px，B进程中的字符串保存虚拟页ux中，对应的物理页为qx。如果将虚拟页ux所映射的物理页改为px，就可以完成对A进程中页的窥探。

观察实验所需要完成的系统调用：

```c
SYSCALL_DEFINE3(peep_page, pid_t, tar_pid_nr, unsigned long, tar_addr, unsigned long, my_addr)
```

注意到共要传入三个参数，目标进程的pid，要窥探的页的目标地址，本进程需要被替换的页的目标地址。

希望将A进程的页将B进程的页覆盖，就需要分别得到A进程和B进程页表所在的地址，实验提供的函数`addr_to_pte()`可以完成这个任务。

```c
static pte_t*
addr_to_pte(struct mm_struct *mm, unsigned long addr)
{
	return pte_offset_kernel(pmd_off(mm, addr), addr);
}
```

在amd64架构上，页表通常分为四级：页全局目录（PGD）、页上四级目录（P4D）、页上三级目录（PUD）、页中间目录（PMD）和页表（PTE）。  分析该函数，可以发现其调用了`pmd_off()`和`pte_offset_kernel()`函数，这两个函数的定义可以在`include/linux/pgtable.h`中找到，

```c
static inline pmd_t *pmd_off(struct mm_struct *mm, unsigned long va)
{
	return pmd_offset(pud_offset(p4d_offset(pgd_offset(mm, va), va), va), va);
}

static inline pte_t *pte_offset_kernel(pmd_t *pmd, unsigned long address)
{
	return (pte_t *)pmd_page_vaddr(*pmd) + pte_index(address);
}
```

可以看到，通过逐层的调用`p*d_offset()`函数，就可以从`mm_struct`中的pgd指针得到表示页表的指针`pte_t*`，通过这个方法就可以得到A进程和B进程的页表。

### 实现

知道如何从地址得到页表之后，就可以开始着手编写系统调用，观察到`addr_to_pte()`函数还需要获取A进程和B进程的`mm_struct`。  B进程由于是本系统调用的调用者，使用`current->mm`即可获取`mm_struct`，主要问题即为如何获取A进程的`mm_struct`。

系统调用传入的A进程的pid，通过A进程的pid，使用`find_get_pid()`函数可以得到A进程的`strcut pid*`(pid结构体)，通过这个结构体调用`pid_task()`函数，就可以得到这个进程的`task_struct`。

得到`task_struct`后，使用`task_struct->mm`就可以访问其`mm_struct`。

通过`mm_struct`和地址得到`pte_t*`后，就可以使用`set_pte()`函数，将A进程的页表覆盖到B进程的页表上。

理论上来说，完成这部操作后就完成了任务，但是实际上还有几步额外的操作需要完成。

执行`flush_tlb_local()`函数，刷新TLB让上述的更改生效。

同时刚刚调用`find_get_pid()`时，增加了对找到的`pid*`的应用计数，需要调用`put_pid()`释放。

至此任务完成。

### 重新编译安装内核

完成相关实现后，要重新编译安装系统内核，见根目录`README.md`。