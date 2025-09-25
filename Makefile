# Makefile for the game

# Detech OS
UNAME_S := $(shell uname -s)

# Executable name
TARGET = shooter

# Compiler
CXX = g++

# Compiler flags
# -Isrc: Add src to include path
# -MMD -MP: Generate dependency files
CXXFLAGS = -std=c++17 -Wall -Isrc -MMD -MP

# Source files
SRCS = src/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Dependency files
DEPS = $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# OS-specific configuration
ifeq ($(UNAME_S), Linux)
    LIBS = $(shell sdl2-config --libs)
    CXXFLAGS += $(shell sdl2-config --cflags)
    RM = rm -f
endif

ifeq ($(UNAME_S), Darwin) # macOS
    LIBS = $(shell sdl2-config --libs)
    CXXFLAGS += $(shell sdl2-config --cflags)
    RM = rm -f
endif

ifeq ($(OS), Windows_NT)
    TARGET := $(TARGET).exe
    # User must set SDL2_PATH to the root of the SDL2 development library
    # e.g. export SDL2_PATH=C:/SDL2-2.0.14
    LIBS = -L$(SDL2_PATH)/lib/x64 -lSDL2main -lSDL2
    CXXFLAGS += -I$(SDL2_PATH)/include
    RM = del /F /Q
endif


# Linker
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

# Include dependency files
-include $(DEPS)

.PHONY: all clean
