# Compiler and flags
CXX = g++
CXXFLAGS = -Ilib -Ithird_party/include -std=c++11 -Wall

# Find all .cpp files in lib and third_party directories
LIB_SRC = $(shell find lib -name '*.cpp')
THIRD_PARTY_SRC = $(shell find third_party -name '*.cpp')

# Add main-related .cpp files explicitly
APP_SRC = RX.cpp TX.cpp

RECV_TARGET = rx
SEND_TARGET = tx

# Combine all source files
SRC = $(LIB_SRC) $(THIRD_PARTY_SRC) $(APP_SRC)

# Replace .cpp with .o and put into build/ while preserving paths
OBJ = $(patsubst %.cpp, build/%.o, $(SRC))

# Default target
all: $(TARGET)

# Executable targets
$(TARGET): build/main.o $(filter-out build/TX.o build/RX.o, $(OBJ))
	$(CXX) $^ -o $@

rx: build/RX.o $(filter-out build/main.o build/TX.o, $(OBJ))
	$(CXX) $^ -o $(RECV_TARGET)

tx: build/TX.o $(filter-out build/main.o build/RX.o, $(OBJ))
	$(CXX) $^ -o $(SEND_TARGET)

# General rule for compiling .cpp to build/*.o
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf build $(TARGET) $(RECV_TARGET) $(SEND_TARGET)
