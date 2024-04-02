CC = g++
CFLAGS = -std=c++11 -pthread
bank: main.o bank.o
	$(CC) $(CFLAGS) main.o bank.o -o Bank 
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp
bank.o: bank.cpp bank.h
	$(CC) $(CFLAGS) -c bank.cpp
clean:
	rm -rf *.o *~ "#"* core.* Bank 
