CXX = clang++
CXXFLAGS = -ggdb
OPT=-lcurl

BIN = main

all: $(BIN)

main: main.cpp
	${CXX} $(CXXFLAGS) $(OPT) $^ -o $@

clean:
	rm -f $(BIN)

.PHONY: all  clean main
