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

# OS-specific configuration

ifeq ($(UNAME_S), Linux)
    LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
    CXXFLAGS += $(shell sdl2-config --cflags) -I$(shell sdl2-config --prefix)/include/SDL2
    RM = rm -f
endif

ifeq ($(UNAME_S), Darwin) # macOS
    LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
    CXXFLAGS += $(shell sdl2-config --cflags)
    RM = rm -f
endif

ifeq ($(OS), Windows_NT)
    TARGET := $(TARGET).exe
    
    # Tự động lấy đường dẫn cài đặt từ Scoop
    SCOOP_SDL2_PATH = ./win-deps/SDL2-2.32.10/x86_64-w64-mingw32/
    SCOOP_IMG_PATH  = ./win-deps/SDL2_image-2.8.10/x86_64-w64-mingw32/
    SCOOP_TTF_PATH  = ./win-deps/SDL2_ttf-2.24.0/x86_64-w64-mingw32/

    # Tổng hợp Include (Dùng dấu / thay vì \ để tránh lỗi escape trong Makefile)
    CXXFLAGS += -I"$(call FIX_PATH,$(SCOOP_SDL2_PATH))/include/SDL2" \
                -I"$(call FIX_PATH,$(SCOOP_IMG_PATH))/include/SDL2" \
                -I"$(call FIX_PATH,$(SCOOP_TTF_PATH))/include/SDL2"

    # Tổng hợp Libs
	LIBS = -L"$(call FIX_PATH,$(SCOOP_SDL2_PATH))/lib" -L"$(call FIX_PATH,$(SCOOP_IMG_PATH))/lib" -L"$(call FIX_PATH,$(SCOOP_TTF_PATH))/lib" \
			-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows
    # Lệnh xóa file trên Windows (dùng del an toàn hơn)
    RM = rm -f
    # Fix đường dẫn cho shell (chuyển \ thành /)
    FIX_PATH = $(subst \,/,$(1))
	COPY_DLLS = cp -f $(call FIX_PATH,$(SCOOP_SDL2_PATH))bin/*.dll . && \
                cp -f $(call FIX_PATH,$(SCOOP_IMG_PATH))bin/*.dll . && \
                cp -f $(call FIX_PATH,$(SCOOP_TTF_PATH))bin/*.dll .
endif

# Default target
all: $(TARGET)

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
ifeq ($(OS), Windows_NT)
	-@rm -f *.dll
endif

# Test rules
test: $(TEST_TARGET)

run-test: test
ifeq ($(OS), Windows_NT)
	@echo "Checking/Copying DLLs..."
	@$(COPY_DLLS)
endif
	./$(TEST_TARGET)

run: all
ifeq ($(OS), Windows_NT)
	@echo "Checking/Copying DLLs..."
	@$(COPY_DLLS)
endif
	./$(TARGET)

# Include dependency files
-include $(DEPS)

.PHONY: all clean test run-test run
