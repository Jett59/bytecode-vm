CXXFLAGS:=-std=c++17 -O3 -I include -Wall -Wextra -pedantic

SRCS:=$(shell find src -name *.cpp)

vm: $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@
