// Readme for CS3013 Operating Systems Project 2
// Alec Benson
// Peter Salem


-------------------------
To Compile:
-------------------------
	type 'make'

To load the module:
	type 'make insmod'

To unload the moduke:
	type 'make rmmod'


-------------------------
To Test:
-------------------------

You will need to create a user whose processes you will smite. 
To do this, you make type:

	adduser testuser

press enter a bunch of times. The terminal will prompt about passwords/contact info. None of this matters.

Then, we need to determine the UID of our testuser:

	id testuser

In our tests, testuser had a UID of 1002, although this will likely vary depending on the number of users that are already present on the VM. 


Now, we need to open up a new terminal, and su to the testuser:

	sudo su testuser

when we have switched to testuser, we may begin testing our smite command.
In another terminal, cd to the included "tests" directory and make the test programs:

	make all

Then, su as root:

	sudo su root

In the original terminal, where you have switched to testuser, run the following test program:

	time ./testloop

This program will simply count to 2000000000 and return. On my system, the program finished executing in about 3 seconds, so once you type this command, you must be quick to run the smite command before it finishes:

	./test UIDofTestuser
	(we ran ./test 1002, for example)

After you have started the test, you will see that the terminal has smited the process, and is counting down for 5 seconds. After five seconds, the program will be unsmited and continue where it left off. In our tests, the program finished in about 8 seconds, instead of the normal 3.

To confirm that the process was in fact smited, we can check the syslogs:

	make syslog


Example syslog Output:

Feb  6 21:27:49 alec-Ubuntu kernel: [  289.263464] SMITE - process 3196 with state 0
Feb  6 21:27:54 alec-Ubuntu kernel: [  294.265695] UNSMITE - process 3196


-------------------------

Thanks!




