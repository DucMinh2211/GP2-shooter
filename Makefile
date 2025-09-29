# Makefile for the game

# Detech OS
UNAME_S := $(shell uname -s)

# Executable name
TARGET = shooter
TEST_TARGET = test-char

# Compiler
CXX = g++

# Compiler flags
# -Isrc: Add src to include path
# -MMD -MP: Generate dependency files
CXXFLAGS = -std=c++17 -Wall -Isrc -MMD -MP

# Source files
SRCS = $(filter-out src/main.cpp, $(shell find src -name '*.cpp'))
MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_char.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Dependency files
DEPS = $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

# Default target
all: $(TARGET)

# OS-specific configuration

ifeq ($(UNAME_S), Linux)
    LIBS = $(shell sdl2-config --libs) -lSDL2_image
    CXXFLAGS += $(shell sdl2-config --cflags)
    RM = rm -f
endif

ifeq ($(UNAME_S), Darwin) # macOS
    LIBS = $(shell sdl2-config --libs) -lSDL2_image
    CXXFLAGS += $(shell sdl2-config --cflags | sed 's|/SDL2||g')
    RM = rm -f
endif

ifeq ($(OS), Windows_NT)
    TARGET := $(TARGET).exe
    # User must set SDL2_PATH to the root of the SDL2 development library
    # e.g. export SDL2_PATH=C:/SDL2-2.0.14
    LIBS = -L$(SDL2_PATH)/lib/x64 -lSDL2main -lSDL2 -lSDL2_image
    CXXFLAGS += -I$(SDL2_PATH)/include
    RM = del /F /Q
endif


# Linker
$(TARGET): $(OBJS) $(MAIN_OBJ)
	$(CXX) $(OBJS) $(MAIN_OBJ) -o $(TARGET) $(LIBS)

$(TEST_TARGET): $(OBJS) $(TEST_OBJ)
	$(CXX) $(OBJS) $(TEST_OBJ) -o $(TEST_TARGET) $(LIBS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	$(RM) $(TARGET) $(TEST_TARGET) $(OBJS) $(MAIN_OBJ) $(TEST_OBJ) $(DEPS)

# Test rules
test: $(TEST_TARGET)

run-test: test
	./$(TEST_TARGET)

# Include dependency files
-include $(DEPS)

.PHONY: all clean test run-test
