# Compiler & Flags
CC = g++
CFLAGS = -std=c++20 -O3
DEBUG_CFLAGS = -std=c++20 -O0 -g

# Build Mode (release by default, override with `make debug=1`)
ifeq ($(debug),1)
	CURRENT_CFLAGS := $(DEBUG_CFLAGS)
else
	CURRENT_CFLAGS := $(CFLAGS)
endif

# Source Files
SRC = ./src/main.cpp ./src/helpers.cpp ./src/orderbook.cpp
UNIT_TEST_SRC = ./src/unit_tests.cpp ./src/helpers.cpp ./src/orderbook.cpp
BENCHMARK_SRC = ./src/benchmark_orderbook.cpp ./src/helpers.cpp ./src/orderbook.cpp

# Object Files
OBJ = $(SRC:.cpp=.o)
UNIT_TEST_OBJ = $(UNIT_TEST_SRC:.cpp=.o)
BENCHMARK_OBJ = $(BENCHMARK_SRC:.cpp=.o)

# Targets
TARGET = main
UNIT_TEST_TARGET = unit_tests
BENCHMARK_TARGET = benchmark_orderbook

# Default build all
all: $(TARGET) $(UNIT_TEST_TARGET) $(BENCHMARK_TARGET)

# Link the main executable
$(TARGET): $(OBJ)
	$(CC) $(CURRENT_CFLAGS) -o $@ $(OBJ)

# Link the unit tests executable
$(UNIT_TEST_TARGET): $(UNIT_TEST_OBJ)
	$(CC) $(CURRENT_CFLAGS) -o $@ $(UNIT_TEST_OBJ)

# Link the benchmark executable
$(BENCHMARK_TARGET): $(BENCHMARK_OBJ)
	$(CC) $(CURRENT_CFLAGS) -o $@ $(BENCHMARK_OBJ)

# Compile rule for .o from .cpp
%.o: %.cpp
	$(CC) $(CURRENT_CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(UNIT_TEST_OBJ) $(BENCHMARK_OBJ) \
		  $(TARGET) $(UNIT_TEST_TARGET) $(BENCHMARK_TARGET)

# Phony target to prevent filename conflict
.PHONY: clean
