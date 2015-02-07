#include <sys/syscall.h>
#include <stdio.h>
// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 355

long testCall1 ( void) {
	return (long) syscall(__NR_cs3013_syscall1);
}
int main () {
	printf("The return values of the system calls are:\n");
	printf("\tcs3013_syscall1: %ld\n", testCall1());
	return 0;
}