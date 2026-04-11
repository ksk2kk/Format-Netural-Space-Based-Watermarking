CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pthread

SRCDIR = src
BINDIR = bin

TARGET = $(BINDIR)/advanced_main

all: $(BINDIR) $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(SRCDIR)/advanced_main.cpp $(SRCDIR)/compare_module.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/advanced_main.cpp -o $(TARGET)

clean:
	rm -rf $(BINDIR)
