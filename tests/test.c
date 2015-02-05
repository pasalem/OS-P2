#include <sys/syscall.h>
#include <stdio.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 355
#define __NR_cs3013_syscall2 356
#define __NR_cs3013_syscall3 357
#define num_processes 100

long smite (unsigned short* target_uid, int* num_pids_smited, int* smited_pids, long* pid_states) {
	return (long) syscall(__NR_cs3013_syscall2, target_uid, num_pids_smited, smited_pids, pid_states);
}
long unsmite (int* num_pids_smited, int* smited_pids, long* pid_states) {
	return (long) syscall(__NR_cs3013_syscall3, num_pids_smited, smited_pids, pid_states);
}


int main (int argc, char* argv[]) {
	int num_pids_smited;
	unsigned short target_uid;
	int smited_pids[num_processes];
	long pid_states[num_processes];

	target_uid = atoi(argv[1]);


	printf("The return values of the system calls are:\n");
	printf("\tSmite: %ld\n", smite(&target_uid, &num_pids_smited, smited_pids, pid_states));
	printf("\tUnsmite: %ld\n", unsmite(&num_pids_smited, smited_pids, pid_states));
	return 0;
}