all: QuadTreeCompressor.exe

imageIO.o:
	gcc -c src/imageIO.c -Iinclude -o imageIO.o

QuadTree.o:
	gcc -c src/QuadTree.c -Iinclude -o QuadTree.o

QuadTreeCompressor.exe: src/main.c imageIO.o QuadTree.o
	gcc src/main.c imageIO.o QuadTree.o -Iinclude -o Compress.exe

clean:
	del *.exe
	del *.o