CXX = g++
CFLAGS = -pthread -std=c++17 -Wall

SOURCES = crawler.cpp ../SetList.cpp ../SafeUnboundedQueue.cpp
OBJECTS = crawler.o

crawler: $(OBJECTS)
	$(CXX) $(CFLAGS) -o crawler $(OBJECTS) -lcurl

crawler.o: crawler.cpp
	$(CXX) -c $(CFLAGS) -o crawler.o crawler.cpp 

clean:
	rm -f *.o
	rm -f crawler
