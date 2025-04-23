# Compiler
CXX = g++
CXXFLAGS = -Wall -Icomponents/header -Isrc/include/SDL2 -Isrc/include/json -Icomponents/header

# SDL2 Libraries
LDFLAGS = -Lsrc/lib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -mconsole

# Source files
SRC_DIRS = components/source
SRC = $(wildcard $(SRC_DIRS)/*.cpp) main.cpp

# Output
TARGET = main.exe

# Suppress unnecessary output
.SILENT:

# Default target
all: $(TARGET)

# Link and compile source files to create executable
$(TARGET): $(SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

# Clean build files
clean:
	@if exist $(TARGET) del /Q $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run