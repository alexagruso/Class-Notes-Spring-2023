This program simulates a workload of 10,000 processes, along with distributing
the processes to multiple CPUs.

To execute the program, run the following commands in the directory:

$ make
$ ./simulator [args]

The program takes four arguments:
./simulator [average arrival time] [average service time] [cpu strategy] [cpu count]