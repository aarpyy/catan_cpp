CC = g++ -Wall -Wextra -std=c++11
SDL_flags = `sdl2-config --cflags --libs` -L./SDL_ttf/.libs/ -lSDL2_ttf

all: Game

Game: Game.cpp Game.h Board.o Player.o Vertex.o Tile.o GameLoader.o
	$(CC) -o Game Game.cpp GameLoader.o Board.o Player.o Vertex.o Tile.o $(SDL_flags)

main: main.cpp Board.o Player.o Vertex.o Tile.o GameLoader.o
	$(CC) -o main main.cpp GameLoader.o Board.o Player.o Vertex.o Tile.o `sdl2-config --cflags --libs` -lSDL2_ttf

GameLoader.o: GameLoader.cpp Tile.h Tile.cpp Board.h Board.cpp
	$(CC) -c GameLoader.cpp

Board.o: Board.cpp Board.h Player.cpp Player.h Vertex.cpp Vertex.h Tile.cpp Tile.h
	$(CC) -c Board.cpp

Player.o: Player.cpp Player.h Vertex.cpp Vertex.h Tile.cpp Tile.h
	$(CC) -c Player.cpp

Vertex.o: Vertex.cpp Vertex.h Tile.cpp Tile.h
	$(CC) -c Vertex.cpp

Tile.o: Tile.cpp Tile.h
	$(CC) -c Tile.cpp

clean:
	@find . -name '*.o' | xargs rm -f
	@rm -f main

.PHONY: all clean
