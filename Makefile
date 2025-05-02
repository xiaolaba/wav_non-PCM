#xiaolaba, 2025-MAY-01
#minGW Makefile


# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -Wall -Wextra -O2

# Project structure
TARGET = wav_rw.exe
#SRC_DIR = src
SRC_DIR = ./
OBJ_DIR = obj
INPUTS_DIR = inputs
OUTPUTS_DIR = outputs

# Source files (update these with your actual filenames)
#C_SOURCES = $(SRC_DIR)/wav.c
CPP_SOURCES = $(SRC_DIR)/main.cpp


# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
CPP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)

# Default target
all: create_dir $(TARGET)

# Create obj directory if needed
create_dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(INPUTS_DIR)
	@mkdir -p $(OUTPUTS_DIR)
	

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile C sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all create_dir clean