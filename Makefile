EXE ?= Spaghet
CXX ?= clang++

compile: 
	@clang++ src/*.cpp -o $(EXE).exe -O3 -march=native

clean: 
	@del /S *.exe