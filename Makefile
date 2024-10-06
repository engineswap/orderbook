# Makefile for a multi-file C++ project with unit tests

# Variables
CC = g++
CFLAGS = -std=c++17
SRC = ./src/main.cpp ./src/helpers.cpp ./src/orderbook.cpp
UNIT_TEST_SRC = ./src/unit_tests.cpp ./src/helpers.cpp ./src/orderbook.cpp
OBJ = $(SRC:.cpp=.o)
UNIT_TEST_OBJ = $(UNIT_TEST_SRC:.cpp=.o)
TARGET = main
UNIT_TEST_TARGET = unit_tests

# Default target builds both main program and unit tests
all: $(TARGET) $(UNIT_TEST_TARGET)

# Linking the main executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Linking the unit tests executable
$(UNIT_TEST_TARGET): $(UNIT_TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(UNIT_TEST_OBJ)

# Compiling source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(UNIT_TEST_OBJ) $(TARGET) $(UNIT_TEST_TARGET)

# Phony target to prevent conflicts with file names
.PHONY: clean
