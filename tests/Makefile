all: test.o
	gcc -o test test.o -I.
clean:
	rm *.o
syslog:
	tail -n 10 /var/log/syslog