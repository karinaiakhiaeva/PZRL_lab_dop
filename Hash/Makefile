CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = test_hashtable

all: $(TARGET)

$(TARGET): Hash.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) Hash.o -L. -lHashTest

Hash.o: Hash.cpp HashTable.h
	$(CXX) $(CXXFLAGS) -c Hash.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean run
