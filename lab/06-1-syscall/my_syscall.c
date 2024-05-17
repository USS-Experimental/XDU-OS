#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mutex.h>
#include <linux/sched.h>

static DEFINE_MUTEX(my_syscall_mutex); // Set up a mutex

SYSCALL_DEFINE2(my_syscall, int, num, int, flag)
{
    int result;

    if (!mutex_trylock(&my_syscall_mutex)) { // Try to get the lock
        printk(KERN_INFO "my_syscall: Another process is using this syscall.\n");
        return -EBUSY;
    }

    // Entering critical section
    printk(KERN_INFO "my_syscall: Process %d is in the critical section.\n", current->pid);
    if (flag == 0)
    {
        result = num % 10; // If flag is 0 get the units digit
    }
    else if (flag == 1)
    {
        result = (num / 10) % 10; // If flag is 1 get the tens digit
    }
    else
    {
        printk(KERN_INFO "my_syscall: Invalid flag.\n");
    }
    // Leaving critical section

    mutex_unlock(&my_syscall_mutex); // Release the lock

    return result;
}