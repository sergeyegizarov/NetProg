CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra
TARGET = daytime_cli
SRC = daytime_cli.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<
	
clean:
	rm -f $(TARGET)
	
.PHONY: all clean
