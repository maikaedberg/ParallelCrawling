CXX = g++
CFLAGS = -pthread -std=c++17 -Wall

SOURCES = benchmarker.cpp ../SetList.cpp ../SafeUnboundedQueue.cpp
OBJECTS = benchmarker.o

benchmarker: $(OBJECTS)
	$(CXX) $(CFLAGS) -o benchmarker $(OBJECTS) -lcurl

benchmarker.o: benchmarker.cpp
	$(CXX) -c $(CFLAGS) -o benchmarker.o benchmarker.cpp 

clean:
	rm -f *.o
	rm -f benchmarker
