CC = gcc

#si vous etes sous linux decomenter les lignes linux et commenter les lignes windows

SRC = vonoroi game SMap utile
HEADR = interface

COMPILER = -c -Wall

#windows
#CFLAGS = -IC:\SDL2\include
#windows

#linux
CFLAGS = $(shell sdl2-config --cflags)
#linux


#windows
#CLIBS = -LC:\SDL2\lib
#windows

#linux
CLIBS = $(shell sdl2-config --libs)
CLIBS += -lm -ldl
#linux

CLIBS += -lSDL2_image -lSDL2_ttf

#windows
#CLIBS += -lmingw32 -lSDL2main -lSDL2
#windows

	
app.exe: main.o utile.o SMap.o voronoi.o game.o
	$(CC) main.o voronoi.o game.o SMap.o utile.o $(CFLAGS) $(CLIBS) -o app.exe
	gcc -c -fPIC $(CFLAGS) ia_test.c utile.c SMap.c
	gcc -shared ia_test.o utile.o SMap.o -o ia_test.so
	
main.o: main.c interface.h utile.c utile.h SMap.c SMap.h voronoi.c voronoi.h SMap.h SMap.c
	$(CC) $(COMPILER) $(CFLAGS) main.c

utile.o: utile.c utile.h
	$(CC) $(COMPILER) $(CFLAGS) utile.c

SMap.o: SMap.c SMap.h utile.h utile.c interface.h
	$(CC) $(COMPILER) $(CFLAGS) SMap.c

voronoi.o: voronoi.c voronoi.h utile.h utile.c
	$(CC) $(COMPILER) $(CFLAGS) voronoi.c

game.o: utile.h utile.c game.c game.h SMap.h SMap.c
	$(CC) $(COMPILER) $(CFLAGS) game.c

debug: clear
debug: COMPILER += -g
debug: app.exe

clear:
	touch app.exe main.o
	rm *.o app.exe
