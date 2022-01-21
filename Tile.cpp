#include "Tile.h"

Tile::Tile(char t, char c) {
    type = t;
    chitNum = c;
    hasRobber = false;
    x = y = 0;
    tileRect = new SDL_Rect;
    chitRect = new SDL_Rect;
}

Tile::Tile() : Tile(BLANK, 0) { }

void Tile::freeTile() const {
    delete tileRect;
    delete chitRect;
}

void *Tile::operator new(size_t size) {
    return ::operator new(size);
}

void Tile::operator delete(void *ptr) {
    Tile *tile = (Tile*)ptr;
    tile->freeTile();
    free(ptr);
}

void Tile::setRect(int _x, int _y, int _w, int _h) {
    x = _x;
    y = _y;

    tileRect->w = _w;
    tileRect->h = _h;

    // Cut in half for adjustment
    _w /= 2;
    _h /= 2;

    tileRect->x = _x - _w;
    tileRect->y = _y - _h;

    // chitRect is square with size 1/3 of tile
    _w = _h = _w / 3;

    chitRect->x = x - _w;
    chitRect->y = y - _w;
    chitRect->w = chitRect->h = (2 * _w);
}

std::pair<int, int> Tile::position(int pos, std::pair<int, int> center, int half_tile, int tile_side) {
    switch (pos) {
        case 0:
            return std::make_pair(center.first - (2 * half_tile), center.second - (3 * tile_side));
        case 1:
            return std::make_pair(center.first, center.second - (3 * tile_side));
        case 2:
            return std::make_pair(center.first + (2 * half_tile), center.second - (3 * tile_side));
        case 3:
            return std::make_pair(center.first - (3 * half_tile), center.second - ((3 * tile_side) / 2));
        case 4:
            return std::make_pair(center.first - half_tile, center.second - ((3 * tile_side) / 2));
        case 5:
            return std::make_pair(center.first + half_tile, center.second - ((3 * tile_side) / 2));
        case 6:
            return std::make_pair(center.first + (3 * half_tile), center.second - ((3 * tile_side) / 2));
        case 7:
            return std::make_pair(center.first - (4 * half_tile), center.second);
        case 8:
            return std::make_pair(center.first - (2 * half_tile), center.second);
        case 9:
            return std::make_pair(center.first, center.second);
        case 10:
            return std::make_pair(center.first + (2 * half_tile), center.second);
        case 11:
            return std::make_pair(center.first + (4 * half_tile), center.second);
        case 12:
            return std::make_pair(center.first - (3 * half_tile), center.second + ((3 * tile_side) / 2));
        case 13:
            return std::make_pair(center.first - half_tile, center.second + ((3*  tile_side) / 2));
        case 14:
            return std::make_pair(center.first + half_tile, center.second + ((3 * tile_side) / 2));
        case 15:
            return std::make_pair(center.first + (3 * half_tile), center.second + ((3 * tile_side) / 2));
        case 16:
            return std::make_pair(center.first - (2 * half_tile), center.second + (3 * tile_side));
        case 17:
            return std::make_pair(center.first, center.second + (3 * tile_side));
        case 18:
            return std::make_pair(center.first + (2 * half_tile), center.second + (3 * tile_side));
        default:
            return std::make_pair(0, 0);
    }
}

std::vector<int> Tile::adjacentVertices(int pos) {
    switch (pos) {
        case 0:
            return {0, 3, 4, 7, 8, 12};
        case 1:
            return {1, 4, 5, 8, 9, 13};
        case 2:
            return {2, 5, 6, 9, 10, 14};
        case 3:
            return {7, 11, 12, 16, 17, 22};
        case 4:
            return {8, 12, 13, 17, 18, 23};
        case 5:
            return {9, 13, 14, 18, 19, 24};
        case 6:
            return {10, 14, 15, 19, 20, 25};
        case 7:
            return {16, 21, 22, 27, 28, 33};
        case 8:
            return {17, 22, 23, 28, 29, 34};
        case 9:
            return {18, 23, 24, 29, 30, 35};
        case 10:
            return {19, 24, 25, 30, 31, 36};
        case 11:
            return {20, 25, 26, 31, 32, 37};
        case 12:
            return {28, 33, 34, 38, 39, 43};
        case 13:
            return {29, 34, 35, 39, 40, 44};
        case 14:
            return {30, 35, 36, 40, 41, 45};
        case 15:
            return {31, 36, 37, 41, 42, 46};
        case 16:
            return {39, 43, 44, 47, 48, 51};
        case 17:
            return {40, 44, 45, 48, 49, 52};
        case 18:
            return {41, 45, 46, 49, 50, 53};
        default:
            return {};
    }
}
