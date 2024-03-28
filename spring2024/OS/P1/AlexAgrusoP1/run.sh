#!/bin/sh

OUTPUT_FILE="output.csv"

# clean and prepare output file
echo "Arrival,Turnaround,Throughput,CpuUtilization,DiskUtilization,CpuWait,DiskWait," > ${OUTPUT_FILE};

# clean and make project
make clean
make

# perform simulations
for i in $(seq 1 30); do
	./simulation ${i} 0.02 0.06 -o >> ${OUTPUT_FILE}
done
