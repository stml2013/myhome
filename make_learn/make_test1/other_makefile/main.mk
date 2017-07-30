main: main.o print.o computer.o
	gcc -o main main.o print.o computer.o
main.o: main.c print.h computer.h
	gcc -c main.c
include pr.mk cm.mk

.PHONY: clean
clean:
	-rm main main.o print.o computer.o
