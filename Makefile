EXE ?= Spaghet
CXX ?= clang++
WARNINGS =-Wall -Wcast-qual -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wlogical-op -Wold-style-cast -Wundef -pedantic

ifeq ($(OS),Windows_NT)
	EXTENSION=.exe
else
	EXTENSION=
endif

compile: 
	$(CXX) src/*.cpp -o $(EXE)$(EXTENSION) -O3 -march=native -std=c++17 $(WARNINGS)

debug: 
		clang++ -fsanitize=undefined,address -fno-omit-frame-pointer -g -std=c++17 src/*.cpp -o $(EXE)$(EXTENSION)
		
clean: 
	del /S *.exe
