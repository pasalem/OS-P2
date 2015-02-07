#include "cs3013p2.h"

unsigned long **sys_call_table;
asmlinkage long (*ref_sys_cs3013_syscall2)(void);
asmlinkage long (*ref_sys_cs3013_syscall3)(void);

asmlinkage long smite(unsigned short *target_uid, int *num_pids_smited, int *smited_pids, long *pid_states){
	int k_index = 0;
	int k_smited_pids[NUMTASKS];
	long k_pid_states[NUMTASKS];
	struct task_struct *current_task;
	struct task_struct *task;
	

	//Ensure user is root
	current_task = get_current();
	if( current_task->real_cred->uid.val != 0 ){
		printk(KERN_INFO "SMITE ABORTED - you must be root.\n");
		return 0;
	}

	//Copy vars to kernel space
	if( copy_from_user(&k_index, num_pids_smited, sizeof(num_pids_smited)) ){
		return EFAULT;
	}
	if( copy_from_user(k_smited_pids, smited_pids, sizeof(int) * NUMTASKS) ){
		return EFAULT;
	}
	if( copy_from_user(k_pid_states, pid_states, sizeof(long) * NUMTASKS) ){
		return EFAULT;
	}
		if( copy_from_user(k_pid_states, pid_states, sizeof(long) * NUMTASKS) ){
		return EFAULT;
	}

	//Go through each task
	for_each_process(task){
		if(k_index < NUMTASKS){
			//We found a task to smite
			if( task->real_cred->uid.val == *target_uid && task->state == TASK_RUNNING){
				printk(KERN_INFO "SMITE - process %d with state %ld\n", task->pid, task->state);
				//Copy the pid and state into our arrays
				k_smited_pids[k_index] = task->pid;
				k_pid_states[k_index] = task->state;

				//SMITE MUAHAHAHA
				set_task_state(task, TASK_UNINTERRUPTIBLE);
				k_index++;
			}
		}
	}

	//Copy vars back to user space
	if( copy_to_user(num_pids_smited, &k_index, sizeof(k_index)) ){
		return EFAULT;
	}
	if( copy_to_user(smited_pids, &k_smited_pids, sizeof(int) * NUMTASKS) ){
		return EFAULT;
	}
	if( copy_to_user(pid_states, &k_pid_states, sizeof(long) * NUMTASKS) ){
		return EFAULT;
	}

	return 1;
}

asmlinkage long unsmite(int *num_pids_smited, int *smited_pids, long *pid_states){
	int k_index;
	int k_smited_pids[NUMTASKS];
	long k_pid_states[NUMTASKS];
	struct task_struct *current_task;
	struct task_struct *task;
	

	//Ensure user is not root
	current_task = get_current();
	if( current_task->real_cred->uid.val != 0 ){
		printk(KERN_INFO "UNSMITE ABORTED - you must be root.\n");
		return 0;
	}

	//Copy vars to kernel space
	if( copy_from_user(&k_index, num_pids_smited, sizeof(num_pids_smited)) ){
		return EFAULT;
	}
	if( copy_from_user(k_smited_pids, smited_pids, sizeof(int) * NUMTASKS) ){
		return EFAULT;
	}
	if( copy_from_user(k_pid_states, pid_states, sizeof(long) * NUMTASKS) ){
		return EFAULT;
	}

	//Go through each task
	for(k_index = 0; k_index < *num_pids_smited; k_index++){
		printk(KERN_INFO "UNSMITE - process %d\n", k_smited_pids[k_index]);
		//Unsmite
		//http://stackoverflow.com/questions/8547332/kernel-efficient-way-to-find-task-struct-by-pid
		task = pid_task(find_vpid( k_smited_pids[k_index] ), PIDTYPE_PID);
		if(task == NULL){
			printk(KERN_INFO "Cannot find task with PID %d\n", k_smited_pids[k_index]);
			return 0;
		}
		wake_up_process(task);
		set_task_state(task, k_pid_states[k_index]);
	}


	//Copy vars back to user space
	if( copy_to_user(num_pids_smited, &k_index, sizeof(k_index)) ){
		return EFAULT;
	}
	if( copy_to_user(smited_pids, &k_smited_pids, sizeof(int) * NUMTASKS) ){
		return EFAULT;
	}
	if( copy_to_user(pid_states, &k_pid_states, sizeof(long) * NUMTASKS) ){
		return EFAULT;
	}

	return 1;
}

static unsigned long **find_sys_call_table(void) {
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;
	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;
		if (sct[__NR_close] == (unsigned long *) sys_close) {
			printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX\n",
			(unsigned long) sct);
			return sct;
		}
		offset += sizeof(void *);
	}
	return NULL;
}

static void disable_page_protection(void) {
	/*
	Control Register 0 (cr0) governs how the CPU operates.
	Bit #16, if set, prevents the CPU from writing to memory marked as
	read only. Well, our system call table meets that description.
	But, we can simply turn off this bit in cr0 to allow us to make
	changes. We read in the current value of the register (32 or 64
	bits wide), and AND that with a value where all bits are 0 except
	the 16th bit (using a negation operation), causing the write_cr0
	value to have the 16th bit cleared (with all other bits staying
	the same. We will thus be able to write to the protected memory.
	It’s good to be the kernel!
	*/
	write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
	/*
	See the above description for cr0. Here, we use an OR to set the
	16th bit to re-enable write protection on the CPU.
	*/
	write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
	/* Find the system call table */
	if(!(sys_call_table = find_sys_call_table())) {
		/* Well, that didn’t work.
		Cancel the module loading step. */
		return -1;
	}

	/* Store a copy of all the existing functions */
	ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
	ref_sys_cs3013_syscall3 = (void *)sys_call_table[__NR_cs3013_syscall3];

	/* Replace the existing system calls */
	disable_page_protection();

	sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)smite;
	sys_call_table[__NR_cs3013_syscall3] = (unsigned long *)unsmite;

	enable_page_protection();
	
	/* And indicate the load was successful */
	printk(KERN_INFO "Loaded interceptor!\n");
	return 0;
}

static void __exit interceptor_end(void) {
	/* If we don’t know what the syscall table is, don’t bother. */
	if(!sys_call_table)
		return;
	
	/* Revert all system calls to what they were before we began. */
	disable_page_protection();
	
	sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
	sys_call_table[__NR_cs3013_syscall3] = (unsigned long *)ref_sys_cs3013_syscall3;
	
	enable_page_protection();
	
	printk(KERN_INFO "Unloaded interceptor!\n");
}


MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
