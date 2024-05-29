/* --------------------------------------------------------------
 * File          : patch.c
 * Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
 * Created on    : <2024-02-01>
 * Last modified : <2024-03-01>
 * LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
 * -------------------------------------------------------------*/

// =================================================
// 1. arch/x86/entry/syscalls/syscall_64.tbl
// 文件末尾添加
548	common peep_page        sys_peep_page



// =================================================
// 2. include/linux/syscalls.h
// 文件末尾最后的#endif前添加
asmlinkage long sys_peep_page(pid_t tar_pid_nr, unsigned long tar_addr, unsigned long my_addr);
#endif



// =================================================
// 3. include/uapi/asm-generic/unistd.h
#undef __NR_syscalls
#define __NR_syscalls 451
// 放在这个下面
#define __NR_peep_page 548
__SYSCALL(__NR_peep_page, sys_peep_page)



// =================================================
// 4. mm/mmap.c
// 在函数 do_mmap() 上面添加
// 具体的实现
static pte_t*
addr_to_pte(struct mm_struct *mm, unsigned long addr)
{
	return pte_offset_kernel(pmd_off(mm, addr), addr);
}

SYSCALL_DEFINE3(peep_page, pid_t, tar_pid_nr, unsigned long, tar_addr, unsigned long, my_addr)
{
	struct pid* tar_pid_struct;
	struct mm_struct* tar_mm;
	struct mm_struct* my_mm;
	pte_t* tar_pte;
	pte_t* my_pte;

	printk("peep_page: running!\n");

	/**
	 * We will get the target pte use target pid and target address
	*/
	tar_pid_struct = find_get_pid(tar_pid_nr);
	tar_mm = pid_task(tar_pid_struct, PIDTYPE_PID)->mm;
	tar_pte = addr_to_pte(tar_mm, tar_addr);

	/**
	 * The peeker can get pte usr current
	*/
	my_mm = current->mm;
	my_pte = addr_to_pte(my_mm, my_addr);

	/**
	 * Now we replace the peeker pte to target pte
	*/
	set_pte(my_pte, *tar_pte);

	/**
	 * Flush the TLB to make the change work
	*/
	flush_tlb_local();

	/**
	 * Release pid struct we find
	*/
	put_pid(tar_pid_struct);

	return 0;
}
