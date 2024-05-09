EXE ?= Spaghet
CXX ?= clang++

compile: 
	$(CXX) src/*.cpp -o $(EXE).exe -O3 -march=native -Wall

clean: 
	del /S *.exe