#ifndef TILE_H
#define TILE_H

#include <array>
#include <iostream>
#include <vector>

#include "CatanRules.h"
#include "game_utility.h"


class Tile {

public:

    char type;          // Resource type
    char chitNum;       // Number that tile gives out resources for when rolled

    // x, y coordinates of center of tile
    int x, y;
    bool hasRobber;

    // SDL_Rect of tile, this has width/height info alongside x, y of top left of rect; rect of chit number
    SDL_Rect *tileRect, *chitRect;
    std::vector<int> vertices{};

    Tile(char t, char c);
    Tile();

    void freeTile() const;

    void *operator new (size_t size);
    void operator delete(void *ptr);

    void setRect(int x, int y, int w, int h);

    static std::pair<int, int> position(int pos, std::pair<int, int> center, int half_tile, int tile_side);
    static std::vector<int> adjacentVertices(int pos);
};

#endif //TILE_H
