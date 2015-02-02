obj-m := cs3013_syscall1_switch.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
syslog:
	tail -n 10 /var/log/syslog
insmod:
	sudo insmod cs3013_syscall1_switch.ko
rmmod:
	sudo rmmod cs3013_syscall1_switch.ko
