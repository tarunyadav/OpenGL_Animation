all: fountain

fountain: AirFountain.o camera.o main.o pool.o textures.o vectors.o
	g++   AirFountain.o camera.o main.o pool.o textures.o vectors.o -o fountain -lglut -lGLU

AirFountain.o: AirFountain.cpp
	g++ -c AirFountain.cpp

camera.o: camera.cpp
	g++ -c camera.cpp

main.o: main.cpp
	g++ -c main.cpp

pool.o: pool.cpp
	g++ -c pool.cpp

textures.o: textures.cpp
	g++ -c textures.cpp -fpermissive

vectors.o: vectors.cpp
	g++ -c vectors.cpp

clean:
	rm -rf *.o fountain

