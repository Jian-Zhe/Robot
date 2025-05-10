# Compiler and flags
CXX = g++
CXXFLAGS = -Ilib -std=c++11 -Wall

# Find all .cpp files in the lib directory (recursively)
LIB_SRC = $(wildcard lib/**/*.cpp)

# Add recv.cpp and send.cpp explicitly
SRC = $(LIB_SRC)

# Generate object file names from source files
OBJ = $(patsubst %.cpp, %.o, $(SRC))

# Output executables
TARGET = mainExecutable
RECV_TARGET = rxExe
SEND_TARGET = txExe

# Build rules
all: $(TARGET)

rx: $(OBJ) RX.o
	$(CXX) $(OBJ) RX.o -o $(RECV_TARGET)

tx: $(OBJ) TX.o
	$(CXX) $(OBJ) TX.o -o $(SEND_TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main.o TX.o RX.o $(TARGET) $(RECV_TARGET) $(SEND_TARGET)