EXE ?= Spaghet
CXX ?= clang++

compile: 
	@clang++ src/*.cpp -o $(EXE).exe -march=native

clean: 
	@del /S *.exe