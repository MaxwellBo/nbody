C_FILES := $(wildcard src/*.cpp)
H_FILES := $(wildcard src/*.h)
O_FILES := $(notdir $(C_FILES:%.cpp=%.o))
CC := mpicxx -g -lstdc++ -Wall -pedantic -Wextra -std=c++11 -lm -O3 -march=native -fopenmp
EXE := nbody

all: $(O_FILES)
	$(CC) $^ -o $(EXE)

$(O_FILES): $(C_FILES)
	$(CC) -c $^

clean:
	@rm -f *.o
	@rm -f $(EXE)
