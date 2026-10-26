CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra
TARGET = echo_cli
SRC = echo_cli.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<
	
clean:
	rm -f $(TARGET)
	
.PHONY: all clean
