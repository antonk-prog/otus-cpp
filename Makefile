CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
BIN_DIR := bin

TARGETS := $(BIN_DIR)/mapper_mean $(BIN_DIR)/reducer_mean \
           $(BIN_DIR)/mapper_variance $(BIN_DIR)/reducer_variance

.PHONY: all clean test

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/mapper_mean: src/mapper_mean.cpp src/mapper_common.h src/common.h | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ src/mapper_mean.cpp

$(BIN_DIR)/mapper_variance: src/mapper_variance.cpp src/mapper_common.h src/common.h | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ src/mapper_variance.cpp

$(BIN_DIR)/reducer_mean: src/reducer_mean.cpp src/common.h | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ src/reducer_mean.cpp

$(BIN_DIR)/reducer_variance: src/reducer_variance.cpp src/common.h | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ src/reducer_variance.cpp

test: all
	python3 tests/test_mapreduce.py

clean:
	rm -rf $(BIN_DIR)
