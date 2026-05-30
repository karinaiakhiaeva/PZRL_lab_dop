CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = test_rbtree

all: $(TARGET)

$(TARGET): BST.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) BST.o -L. -lRBTreeTest

BST.o: BST.cpp BST.h
	$(CXX) $(CXXFLAGS) -c BST.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean run
