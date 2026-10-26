CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra
TARGET = echo_srv
SRC = echo_srv.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<
	
clean:
	rm -f $(TARGET)
	
.PHONY: all clean
