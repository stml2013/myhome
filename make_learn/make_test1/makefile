
#简化
objects = main.o print.o computer.o
main : $(objects)
	gcc  $^ -o main

	@echo "main.c is compling"

main.o : print.h computer.h
	gcc -c main.c
	

	@echo "print.c is compling"
print.o : print.h
	gcc -c print.c
	

	@echo "computer.c is comping"

computer.o : computer.h
	gcc -c computer.c
	
.PHONY: clean
clean:
	-rm main $(objects)

main : main.o print.o computer.o
       gcc -o main main.o print.o computer.o
main.o : main.c print.h computer.h
       gcc -c main.c
print.o : print.c print.h
       gcc -c print.c
computer.o : computer.c computer.h
       gcc -c computer.c


