CC = gcc --std=gnu99 -pthread -g
exe_file = line_processor
$(exe_file): main.o processing.o buffers.o
	$(CC) main.o processing.o buffers.o -o $(exe_file)

processing.o: processing.c processing.h
	$(CC) -c processing.c
buffers.o: buffers.c buffers.h
	$(CC) -c buffers.c
main.o: main.c main.h
	$(CC) -c main.c

clean:
	rm *.o $(exe_file)

tar:
	tar -cvf $(exe_file).tar *.c *.h Makefile