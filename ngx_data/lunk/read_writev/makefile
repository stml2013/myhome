
writev: writev.o
	gcc -g -Wall -Wextra $^ -o $@
writev.o: writev.c
	gcc -c -g -Wall -Wextra $<

clean:
	rm -i -f writev *.o
