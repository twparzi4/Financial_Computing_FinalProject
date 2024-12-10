CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -pthread

# 编译主项目的可执行文件
FinalProject: main.o Retriever.o Stock.o
	$(CC) $(CFLAGS) -o FinalProject main.o Retriever.o Stock.o

# 编译 main0.cpp 的可执行文件
TestProject: main0.o Retriever.o Stock.o
	$(CC) $(CFLAGS) -o TestProject main0.o Retriever.o Stock.o

# main.cpp 的目标文件规则
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

# main0.cpp 的目标文件规则
main0.o: main0.cpp
	$(CC) $(CFLAGS) -c main0.cpp

# Retriever.cpp 的目标文件规则
Retriever.o: Retriever.cpp Retriever.h
	$(CC) $(CFLAGS) -c Retriever.cpp

# Stock.cpp 的目标文件规则
Stock.o: Stock.cpp Stock.h
	$(CC) $(CFLAGS) -c Stock.cpp

# 清理命令
clean:
	rm -f FinalProject TestProject *.o