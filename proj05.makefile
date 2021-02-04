proj05: proj05.student.o
	g++ -lpthread proj05.student.o -o proj05

proj05.student.o: proj05.student.c
	g++ -Wall -c -g proj05.student.c 
	
clean:
	rm -f proj05.student.o
	rm -f proj05


