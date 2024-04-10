clang: 
	@clang++ src/*.cpp -march=native
	@./a

gcc:
	@g++ src/*.cpp -o main -march=native
	@./main

clean: 
	@del /S *.exe