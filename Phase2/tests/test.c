#include <sys/syscall.h>
#include <stdio.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 355
#define __NR_cs3013_syscall2 356
#define __NR_cs3013_syscall3 357
#define num_processes 100
#define UNSMITE_DELAY 5

long smite (unsigned short* target_uid, int* num_pids_smited, int* smited_pids, long* pid_states) {
	return (long) syscall(__NR_cs3013_syscall2, target_uid, num_pids_smited, smited_pids, pid_states);
}
long unsmite (int* num_pids_smited, int* smited_pids, long* pid_states) {
	return (long) syscall(__NR_cs3013_syscall3, num_pids_smited, smited_pids, pid_states);
}


int main (int argc, char* argv[]) {
	int num_pids_smited = 0;
	unsigned short target_uid;
	int smited_pids[num_processes];
	long pid_states[num_processes];
	int counter = 0;

	if(argc < 2){
		printf("Include a PID argument when executing the test");
		return 0;
	}
	target_uid = atoi(argv[1]);


	printf("The return values of the system calls are:\n");
	printf("\tSmite: %ld\n", smite(&target_uid, &num_pids_smited, smited_pids, pid_states));
	for(counter = 0; counter < UNSMITE_DELAY; counter++){
		printf("\tUnsmiting in %d seconds\n", UNSMITE_DELAY - counter);
		sleep(1);
	}
	printf("Smited a total of %d processes\n", num_pids_smited);
	printf("\tUnsmite: %ld\n", unsmite(&num_pids_smited, smited_pids, pid_states));
	return 0;
}