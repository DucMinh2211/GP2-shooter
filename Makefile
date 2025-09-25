# Makefile for the game

# Detech OS
UNAME_S := $(shell uname -s)

# Executable name
TARGET = shooter

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall

# Source files
SRCS = src/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# OS-specific configuration
ifeq ($(UNAME_S), Linux)
	LIBS = $(shell sdl2-config --libs)
	CXXFLAGS += $(shell sdl2-config --cflags)
else
    $(error This Makefile is only for Linux)
endif


# Linker
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
