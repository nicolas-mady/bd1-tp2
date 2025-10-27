# Makefile for TP2 - BD1 Project
# Automated build system for C++ programs: upload, findrec, seek1, seek2

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -g
LDFLAGS = 

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
DATA_DIR = data
OBJ_DIR = obj

# Docker image name
DOCKER_IMAGE = tp2-bd1
DOCKER_TAG = latest

# Automatically detect all source files and programs
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
PROGRAMS = $(basename $(notdir $(SOURCES)))
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Dependencies detection
DEPS = $(OBJECTS:.o=.d)

# Create necessary directories
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR) $(DATA_DIR)/db)

# Default target
.PHONY: all
all: preprocess-csv build

# Preprocess CSV file (run once before using the data)
.PHONY: preprocess-csv
preprocess-csv:
	@if [ -f "$(DATA_DIR)/artigo.csv" ]; then \
		echo "Preprocessing CSV file to fix line breaks..."; \
		sed -i ':a;N;$$!ba;s/\n[a-z]//g' $(DATA_DIR)/artigo.csv; \
		echo "CSV preprocessing completed"; \
	else \
		echo "Warning: $(DATA_DIR)/artigo.csv not found"; \
	fi

# Build all programs
.PHONY: build
build: $(addprefix $(BIN_DIR)/, $(PROGRAMS))

# Automatic dependency generation
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MM -MT '$(@:.d=.o)' $< > $@

# Include dependency files
-include $(DEPS)

# Build object files with automatic dependency tracking
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

# Build individual programs (each program is self-contained)
$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Quick build targets for individual programs
.PHONY: upload findrec seek1 seek2
upload: $(BIN_DIR)/upload
findrec: $(BIN_DIR)/findrec
seek1: $(BIN_DIR)/seek1
seek2: $(BIN_DIR)/seek2

# Run targets (automatically build if needed)
.PHONY: run-upload run-findrec run-seek1 run-seek2
run-upload: $(BIN_DIR)/upload
	@echo "Running upload..."
	@cd $(DATA_DIR) && ../$(BIN_DIR)/upload

run-findrec: $(BIN_DIR)/findrec
	@echo "Running findrec..."
	@echo "Usage: run-findrec ID=<record_id>"
	@if [ -n "$(ID)" ]; then \
		cd $(DATA_DIR) && ../$(BIN_DIR)/findrec $(ID); \
	else \
		echo "Please specify ID parameter: make run-findrec ID=123"; \
	fi

run-seek1: $(BIN_DIR)/seek1
	@echo "Running seek1..."
	@echo "Usage: run-seek1 ID=<record_id>"
	@if [ -n "$(ID)" ]; then \
		cd $(DATA_DIR) && ../$(BIN_DIR)/seek1 $(ID); \
	else \
		echo "Please specify ID parameter: make run-seek1 ID=123"; \
	fi

run-seek2: $(BIN_DIR)/seek2
	@echo "Running seek2..."
	@echo "Usage: run-seek2 ID=<record_id>"
	@if [ -n "$(ID)" ]; then \
		cd $(DATA_DIR) && ../$(BIN_DIR)/seek2 $(ID); \
	else \
		echo "Please specify ID parameter: make run-seek2 ID=123"; \
	fi

# Development targets
.PHONY: debug
debug: CXXFLAGS += -DDEBUG -O0
debug: clean build

.PHONY: release
release: CXXFLAGS += -DNDEBUG -O3
release: clean build

# Setup and initialization
.PHONY: setup
setup:
	@echo "Setting up project environment..."
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(DATA_DIR)/db
	@echo "Checking for required files..."
	@if [ ! -f "$(DATA_DIR)/artigo.csv" ]; then \
		echo "Warning: $(DATA_DIR)/artigo.csv not found!"; \
		echo "Please place your CSV file in the data directory."; \
	fi
	@echo "Setup completed!"

# Full rebuild
.PHONY: rebuild
rebuild: clean build

# Install dependencies (if any)
.PHONY: install-deps
install-deps:
	@echo "Checking for required dependencies..."
	@which g++ > /dev/null || (echo "Error: g++ not found. Please install build-essential" && exit 1)
	@echo "All dependencies satisfied!"

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

# Development and testing
.PHONY: test
test: build
	@echo "Running basic tests..."
	@echo "Testing build outputs..."
	@for prog in $(PROGRAMS); do \
		if [ -f "$(BIN_DIR)/$$prog" ]; then \
			echo "✓ $$prog built successfully"; \
		else \
			echo "✗ $$prog build failed"; \
		fi \
	done

# Static analysis
.PHONY: check
check:
	@echo "Running static analysis..."
	@if which cppcheck > /dev/null 2>&1; then \
		cppcheck --enable=all --std=c++17 -I$(INCLUDE_DIR) $(SRC_DIR)/; \
	else \
		echo "cppcheck not found, skipping static analysis"; \
	fi

# Code formatting
.PHONY: format
format:
	@if which clang-format > /dev/null 2>&1; then \
		echo "Formatting code..."; \
		find $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i; \
		echo "Code formatting completed!"; \
	else \
		echo "clang-format not found, skipping formatting"; \
	fi

# Utility targets
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*
	rm -f $(DATA_DIR)/db/*.dat
	@echo "Clean completed!"

.PHONY: clean-all
clean-all: clean
	@echo "Cleaning all generated files..."
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	rm -f $(DATA_DIR)/db/*

# Information targets
.PHONY: info
info:
	@echo "Project Information:"
	@echo "  Source files: $(SOURCES)"
	@echo "  Programs: $(PROGRAMS)"
	@echo "  Compiler: $(CXX)"
	@echo "  Flags: $(CXXFLAGS)"
	@echo "  Build directory: $(BIN_DIR)"
	@echo "  Object directory: $(OBJ_DIR)"

.PHONY: list-targets
list-targets:
	@echo "Available targets:"
	@$(MAKE) -qp | grep '^[a-zA-Z][^$$#]*:' | cut -d: -f1 | sort | uniq

# Show help
.PHONY: help
help:
	@echo "BD1-TP2 Automated Build System"
	@echo "=============================="
	@echo ""
	@echo "Main targets:"
	@echo "  all                  - Preprocess CSV and build all programs"
	@echo "  build                - Build all programs"
	@echo "  clean                - Remove build artifacts"
	@echo "  setup                - Initialize project environment"
	@echo "  rebuild              - Clean and build"
	@echo ""
	@echo "Individual programs:"
	@echo "  upload, findrec, seek1, seek2 - Build specific program"
	@echo ""
	@echo "Run programs:"
	@echo "  run-upload           - Run upload program"
	@echo "  run-findrec ID=<id>  - Run findrec with record ID"
	@echo "  run-seek1 ID=<id>    - Run seek1 with record ID" 
	@echo "  run-seek2 ID=<id>    - Run seek2 with record ID"
	@echo ""
	@echo "Development:"
	@echo "  debug                - Build with debug flags"
	@echo "  release              - Build optimized release"
	@echo "  test                 - Run basic tests"
	@echo "  check                - Run static analysis"
	@echo "  format               - Format source code"
	@echo ""
	@echo "Docker:"
	@echo "  docker-build         - Build Docker image"
	@echo "  docker-compose-up    - Start services"
	@echo "  docker-compose-down  - Stop services"
	@echo ""
	@echo "Utility:"
	@echo "  info                 - Show project information"
	@echo "  list-targets         - List all available targets"
	@echo "  help                 - Show this help"

# Default target when nothing is specified
.DEFAULT_GOAL := all
