BABELTRACE=/scratch/tracecompass/babeltrace/babeltrace-port-staging
all: writer.exe 

run: all
	LD_LIBRARY_PATH=$(BABELTRACE)/lib ./writer.exe

writer.exe: writer.c Makefile
	gcc -g -O0 writer.c -o writer.exe -L$(BABELTRACE)/lib -lbabeltrace -I$(BABELTRACE)/include

clean:
	$(RM) writer.exe
