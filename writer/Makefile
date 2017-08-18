BABELTRACE=/scratch/tracecompass/babeltrace/babeltrace-port-staging
all: writer.exe 

# Run with paths to dll/so. LD_LIBRARY_PATH for linux, PATH for windows
run: all
	LD_LIBRARY_PATH=$(BABELTRACE)/lib PATH=$(BABELTRACE)/bin:$$PATH ./writer.exe

writer.exe: writer.c Makefile
	gcc -g -O0 writer.c -o writer.exe -L$(BABELTRACE)/lib -lbabeltrace -I$(BABELTRACE)/include

clean:
	$(RM) writer.exe
