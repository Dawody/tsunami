build:
	g++ clk.cpp -o clock.out
	g++ scheduler.cpp -o sch.out -Wno-write-strings
	g++ FilesGenerator.cpp -o Files.out
	g++ processGenerator.cpp -o main.out -Wno-write-strings
	g++ process.cpp -o process.out

clean:
	rm -f *.out
	ipcrm -M 300
	ipcrm -Q 777
	@echo "All Items Cleared"

all: clean build

run:
	./main.out
