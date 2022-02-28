CC = g++ -Wall -Wextra -std=c++11

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
SDL_flags = `sdl2-config --cflags --libs` -L./SDL_ttf/.libs -lSDL2_ttf
else
SDL_flags = `sdl2-config --cflags --libs` -L./SDL_ttf -lSDL2_ttf
endif

all: Game

Game: Game.cpp Game.h Board.o Player.o Vertex.o Tile.o GameLoader.o game_utility.o
	$(CC) -o Game Game.cpp GameLoader.o Board.o Player.o Vertex.o Tile.o game_utility.o $(SDL_flags)

GameLoader.o: GameLoader.cpp Tile.h Tile.cpp Board.h Board.cpp game_utility.cpp game_utility.h
	$(CC) -c GameLoader.cpp

Board.o: Board.cpp Board.h Player.cpp Player.h Vertex.cpp Vertex.h Tile.cpp Tile.h game_utility.cpp game_utility.h
	$(CC) -c Board.cpp

Player.o: Player.cpp Player.h Vertex.cpp Vertex.h Tile.cpp Tile.h game_utility.cpp game_utility.h
	$(CC) -c Player.cpp

Vertex.o: Vertex.cpp Vertex.h Tile.cpp Tile.h game_utility.cpp game_utility.h
	$(CC) -c Vertex.cpp

Tile.o: Tile.cpp Tile.h game_utility.cpp game_utility.h
	$(CC) -c Tile.cpp

game_utility.o: game_utility.cpp game_utility.h
	$(CC) -c game_utility.cpp

clean:
	@find . -name '*.o' | xargs rm -f
	@rm -f Game

.PHONY: all clean
