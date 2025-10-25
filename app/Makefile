# Makefile for TP2 - BD1 Project
# Compiles C++ programs: upload, findrec, seek1, seek2

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
DATA_DIR = data

# Docker image name
DOCKER_IMAGE = tp2-bd1
DOCKER_TAG = latest

# Programs to build
PROGRAMS = upload findrec seek1 seek2

# Preprocess CSV file (run once before using the data)
.PHONY: preprocess-csv
preprocess-csv:
	@echo "Preprocessing CSV file to fix line breaks..."
	sed -i ':a;N;$$!ba;s/\n[a-z]//g' $(DATA_DIR)/artigo.csv
	@echo "CSV preprocessing completed"

# Default target
.PHONY: all
all: preprocess-csv build

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build all programs locally
.PHONY: build
build: $(BIN_DIR) $(addprefix $(BIN_DIR)/, $(PROGRAMS))

# Common object files
COMMON_OBJS = $(SRC_DIR)/artigo.o $(SRC_DIR)/hash_table.o $(SRC_DIR)/btree.o

# Build object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

# Build individual programs (linking with common objects)
$(BIN_DIR)/upload: $(SRC_DIR)/upload.cpp $(COMMON_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $^

$(BIN_DIR)/findrec: $(SRC_DIR)/findrec.cpp $(COMMON_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $^

$(BIN_DIR)/seek1: $(SRC_DIR)/seek1.cpp $(COMMON_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $^

$(BIN_DIR)/seek2: $(SRC_DIR)/seek2.cpp $(COMMON_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $^

# Docker build
.PHONY: docker-build
docker-build:
	docker build -t $(DOCKER_IMAGE):$(DOCKER_TAG) .

# Docker compose shortcuts
.PHONY: docker-compose-up
docker-compose-up:
	docker-compose up

.PHONY: docker-compose-down
docker-compose-down:
	docker-compose down

# Utility targets
.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -f $(DATA_DIR)/db/*.dat
	rm -f $(SRC_DIR)/*.o

.PHONY: test
test: build
	@echo "Running tests..."

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  build                - Compile all programs locally"
	@echo "  docker-build         - Build Docker image"
	@echo "  docker-compose-up    - Start services with docker-compose"
	@echo "  docker-compose-down  - Stop services with docker-compose"
	@echo "  clean                - Remove compiled binaries"
	@echo "  test                 - Run tests"
	@echo "  help                 - Show this help message"
