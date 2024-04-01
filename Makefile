run: 
	@g++ src/*.cpp -o main
	@./main

clean: 
	@del /S *.exe