#ifndef TILE_H
#define TILE_H

#include <array>
#include <iostream>
#include <vector>

#include "CatanRules.h"
#include "game_utility.h"

/*
 * Conditional include, not currently used, SDL2 library definitely preferred but
 * everything still works with local ./SDL library
 *
#if __has_include("SDL2/SDL.h")
#include "SDL2/SDL.h"
#else
#include "./SDL/include/SDL.h"
#endif

#if __has_include("SDL2/SDL_ttf.h")
#include "SDL2/SDL_ttf.h"
#else
#include "SDL_ttf/SDL_ttf.h"
#endif
 */

class Tile {

public:

    char type;          // Resource type
    char chitNum;       // Number that tile gives out resources for when rolled
    int x, y;           // x, y coordinates of center of tile
    bool hasRobber;
    SDL_Rect *tileRect; // SDL_Rect of tile, this has width/height info alongside x, y of top left of rect
    SDL_Texture *chitTexture;
    std::vector<int> vertices{};

    Tile(char t, char c);
    Tile();
    void setRect(int x, int y, int w, int h);

    static std::pair<int, int> position(int pos, std::pair<int, int> center, int half_tile, int tile_side);
    static std::vector<int> adjacentVertices(int pos);
};

#endif //TILE_H
