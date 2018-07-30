C_FILES := $(wildcard src/*.cpp)
H_FILES := $(wildcard src/*.h)
O_FILES := $(notdir $(C_FILES:%.cpp=%.o))
CC := gcc -lstdc++ -Wall -pedantic -Wextra -std=c++11
EXE := center
all: $(O_FILES)
    $(CC) $^ -o $(EXE)
$(O_FILES): $(C_FILES)
    $(CC) -c $^
clean:
    @rm -f *.o
    @rm -f $(EXE)
