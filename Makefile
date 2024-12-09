CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -pthread

FinalProject: main.o Retriever.o Stock.o
	$(CC) $(CFLAGS) -o FinalProject main.o Retriever.o Stock.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

Retriever.o: Retriever.h Retriever.cpp
	$(CC) $(CFLAGS) -c Retriever.cpp

Stock.o: Stock.h Stock.cpp
	$(CC) $(CFLAGS) -c Stock.cpp

clean:
	rm -rf  FinalProject  *.o