EXE ?= Spaghet
CXX ?= clang++

compile: 
	@$(CXX) src/*.cpp -o $(EXE) -O3 -march=native

clean: 
	@del /S *.exe