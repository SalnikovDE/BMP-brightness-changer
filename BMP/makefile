all: bmp

bmp: main.o BMP.o
	g++ main.o BMP.o -o bmp
main.o: main.cpp
	g++ main.cpp -c
BMP.o: BMP.cpp
	g++ BMP.cpp -c
clear:
	rm -rf *.o bmp