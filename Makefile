CXX = g++
CFLAGS = -pthread -std=c++17 -Wall

SOURCES = crawler.cpp SetList.cpp SafeUnboundedQueue.cpp main.cpp
OBJECTS = main.o

main: $(OBJECTS)
	$(CXX) $(CFLAGS) -o main $(OBJECTS) -lcurl

main.o: main.cpp
	$(CXX) -c $(CFLAGS) -o main.o main.cpp 

clean:
	rm -f *.o
	rm -f main