# Makefile for TP2 - BD1 Project
# Compiles C/C++ programs: upload, findrec, seek1, seek2

# Compiler and flags
CXX = g++
CC = gcc
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
CFLAGS = -Wall -Wextra -std=c99 -O2

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

# Default target
.PHONY: all
all: build

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build all programs locally
.PHONY: build
build: $(BIN_DIR) $(addprefix $(BIN_DIR)/, $(PROGRAMS))

# Build individual programs (assuming C++ source files)
$(BIN_DIR)/upload: $(SRC_DIR)/upload.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BIN_DIR)/findrec: $(SRC_DIR)/findrec.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BIN_DIR)/seek1: $(SRC_DIR)/seek1.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $<

$(BIN_DIR)/seek2: $(SRC_DIR)/seek2.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $<

# Alternative rules for C source files (uncomment if needed)
# $(BIN_DIR)/upload: $(SRC_DIR)/upload.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

# $(BIN_DIR)/findrec: $(SRC_DIR)/findrec.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

# $(BIN_DIR)/seek1: $(SRC_DIR)/seek1.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

# $(BIN_DIR)/seek2: $(SRC_DIR)/seek2.c | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $<

# Docker build
.PHONY: docker-build
docker-build:
	docker build -t $(DOCKER_IMAGE):$(DOCKER_TAG) .

# Docker run targets for each program
.PHONY: docker-run-upload
docker-run-upload:
	docker run --rm -it \
		-v $(PWD)/$(DATA_DIR):/data \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		./bin/upload

.PHONY: docker-run-findrec
docker-run-findrec:
	docker run --rm -it \
		-v $(PWD)/$(DATA_DIR):/data \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		./bin/findrec

.PHONY: docker-run-seek1
docker-run-seek1:
	docker run --rm -it \
		-v $(PWD)/$(DATA_DIR):/data \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		./bin/seek1

.PHONY: docker-run-seek2
docker-run-seek2:
	docker run --rm -it \
		-v $(PWD)/$(DATA_DIR):/data \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		./bin/seek2

# Docker compose shortcuts
.PHONY: docker-compose-build
docker-compose-build:
	docker-compose build

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

.PHONY: clean-docker
clean-docker:
	docker rmi $(DOCKER_IMAGE):$(DOCKER_TAG) 2>/dev/null || true

.PHONY: test
test: build
	@echo "Running tests..."
	# Add test commands here

# Extract compressed data if needed
.PHONY: extract-data
extract-data:
	@if [ -f "$(DATA_DIR)/artigo.csv.gz" ]; then \
		echo "Extracting artigo.csv.gz..."; \
		gunzip -k $(DATA_DIR)/artigo.csv.gz; \
	fi

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  build                - Compile all programs locally"
	@echo "  docker-build         - Build Docker image"
	@echo "  docker-run-upload    - Run upload program in Docker"
	@echo "  docker-run-findrec   - Run findrec program in Docker"
	@echo "  docker-run-seek1     - Run seek1 program in Docker"
	@echo "  docker-run-seek2     - Run seek2 program in Docker"
	@echo "  docker-compose-build - Build using docker-compose"
	@echo "  docker-compose-up    - Start services with docker-compose"
	@echo "  docker-compose-down  - Stop services with docker-compose"
	@echo "  clean                - Remove compiled binaries"
	@echo "  clean-docker         - Remove Docker image"
	@echo "  extract-data         - Extract compressed data files"
	@echo "  test                 - Run tests"
	@echo "  help                 - Show this help message"
