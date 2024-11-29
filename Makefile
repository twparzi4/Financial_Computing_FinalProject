CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl 

FinalProject: main.o
	$(CC) $(CFLAGS) -o FinalProject main.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -rf  FinalProject  *.o