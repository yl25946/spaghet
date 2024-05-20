EXE ?= Spaghet
CXX ?= clang++
WARNINGS =-Wall -Wcast-qual -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wlogical-op -Wold-style-cast -Wundef -pedantic

compile: 
	$(CXX) src/*.cpp -o $(EXE).exe -O3 -march=native -std=c++17 $(WARNINGS)

debug: 
	clang++ -fsanitize=undefined,address -fno-omit-frame-pointer -g -std=c++17 src/*.cpp $(EXE).exe

clean: 
	del /S *.exe
