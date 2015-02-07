test: test.o
	gcc -o test test.o -I.

testloop: testloop.o
	gcc -o testloop testloop.o -I.

clean:
	rm *.o
syslog:
	tail -n 10 /var/log/syslog