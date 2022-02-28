#include "Vertex.h"
#include <utility>
#include <algorithm>

// Set side length of vertex binding box to 10 pixels
int Vertex::s_vert_size = 10;

Vertex::Vertex(int x, int y, char port) {
    this->x = x;
    this->y = y;
    this->port = port;
    building = 0;
}

Vertex::Vertex(int x, int y) : Vertex(x, y, 0) { }

Vertex::Vertex(std::pair<int, int> coords, char port) : Vertex(coords.first, coords.second, port) {  }

Vertex::Vertex(char port) : Vertex(0, 0, port) { }

Vertex::Vertex(std::pair<int, int> coords) : Vertex(coords.first, coords.second, 0) { }

Vertex::Vertex() : Vertex(0) { }

bool Vertex::in(int _x, int _y) const {
    return encloses(x, y, s_vert_size, s_vert_size, _x, _y);
}

bool Vertex::isEmpty() const {
    return building == 0;
}

bool Vertex::hasSettlement() const {
    return building == 1;
}

bool Vertex::hasCity() const {
    return building == 2;
}

void Vertex::buildSettlement() {
    building = 1;
}

void Vertex::buildCity() {
    building = 2;
}

int Vertex::buildingValue() const {
    return building;
}

/*
 * Returns all resources around this Vertex in the amounts that would be gained after one
 * round by the building type at this Vertex. This is the main collection method of resources.
 */
std::array<int, N_RESOURCE> Vertex::force(std::array<Tile*, N_TILES> boardTiles) const {
    // Get empty list of resources
    std::array<int, N_RESOURCE> resources{0, 0, 0, 0, 0, 0};
    Tile* tile;
    for (int const& i : tiles) {
        /*
         * t->type returns number 1-6 representing type of resource.
         * By incrementing index of type we are saying that the resource of
         * that index has that value
         */
        tile = boardTiles[i];
        if (!tile->hasRobber) resources[tile->type] += building;
    }
    return resources;
}

/*
 * With reference to the top-left-most vertex, the position of the pos - 1, and the position number,
 * this function returns the x, y coordinates of the vertex at position = pos.
 */
std::pair<int, int> Vertex::position(int half_tile, int tile_side, std::pair<int, int> top_left,
                                     std::pair<int, int> recent, int pos) {

    /*
     * Each of the cases besides default represents the first in a row across the x-axis of the window.
     * Default is an adjustment in the x based the vertex in the most recently generated position (pos-1)
     */
    switch (pos) {
        case 0:
            return std::make_pair(top_left.first, top_left.second);
        case 3:
            return std::make_pair(top_left.first - half_tile, top_left.second + (tile_side / 2));
        case 7:
            return std::make_pair(top_left.first - half_tile, recent.second + tile_side);
        case 11:
            return std::make_pair(top_left.first - (2 * half_tile), recent.second + (tile_side / 2));
        case 16:
            return std::make_pair(top_left.first - (2 * half_tile), recent.second + tile_side);
        case 21:
            return std::make_pair(top_left.first - (3 * half_tile), recent.second + (tile_side / 2));
        case 27:
            return std::make_pair(top_left.first - (3 * half_tile), recent.second + tile_side);
        case 33:
            return std::make_pair(top_left.first - (2 * half_tile), recent.second + (tile_side / 2));
        case 38:
            return std::make_pair(top_left.first - (2 * half_tile), recent.second + tile_side);
        case 43:
            return std::make_pair(top_left.first - half_tile, recent.second + (tile_side / 2));
        case 47:
            return std::make_pair(top_left.first - half_tile, recent.second + tile_side);
        case 51:
            return std::make_pair(top_left.first, recent.second + (tile_side / 2));
        default:
            return std::make_pair(recent.first + (2 * half_tile), recent.second);
    }
}

/*
 * Switch on vertex position to find adjacent vertices. If position not given, empty
 * vertex returned. Returns integer index of vertex, not vertex object or pointer.
 */
std::vector<int> Vertex::adjacentVertex(int pos) {
    switch (pos) {
        case 0:
            return std::vector<int>{3, 4};
        case 1:
            return std::vector<int>{4, 5};
        case 2:
            return std::vector<int>{5, 6};
        case 3:
            return std::vector<int>{0, 7};
        case 4:
            return std::vector<int>{0, 1, 8};
        case 5:
            return std::vector<int>{1, 2, 9};
        case 6:
            return std::vector<int>{2, 10};
        case 7:
            return std::vector<int>{3, 11, 12};
        case 8:
            return std::vector<int>{4, 12, 13};
        case 9:
            return std::vector<int>{5, 13, 14};
        case 10:
            return std::vector<int>{6, 14, 15};
        case 11:
            return std::vector<int>{7, 16};
        case 12:
            return std::vector<int>{7, 8, 17};
        case 13:
            return std::vector<int>{8, 9, 18};
        case 14:
            return std::vector<int>{9, 10, 19};
        case 15:
            return std::vector<int>{10, 20};
        case 16:
            return std::vector<int>{11, 21, 22};
        case 17:
            return std::vector<int>{12, 22, 23};
        case 18:
            return std::vector<int>{13, 23, 24};
        case 19:
            return std::vector<int>{14, 24, 25};
        case 20:
            return std::vector<int>{15, 25, 26};
        case 21:
            return std::vector<int>{16, 27};
        case 22:
            return std::vector<int>{16, 17, 28};
        case 23:
            return std::vector<int>{17, 18, 29};
        case 24:
            return std::vector<int>{18, 19, 30};
        case 25:
            return std::vector<int>{19, 20, 31};
        case 26:
            return std::vector<int>{20, 32};
        case 27:
            return std::vector<int>{21, 33};
        case 28:
            return std::vector<int>{22, 33, 34};
        case 29:
            return std::vector<int>{23, 34, 35};
        case 30:
            return std::vector<int>{24, 35, 36};
        case 31:
            return std::vector<int>{25, 36, 37};
        case 32:
            return std::vector<int>{26, 37};
        case 33:
            return std::vector<int>{27, 28, 38};
        case 34:
            return std::vector<int>{28, 29, 39};
        case 35:
            return std::vector<int>{29, 30, 40};
        case 36:
            return std::vector<int>{30, 31, 41};
        case 37:
            return std::vector<int>{31, 32, 42};
        case 38:
            return std::vector<int>{33, 43};
        case 39:
            return std::vector<int>{34, 43, 44};
        case 40:
            return std::vector<int>{35, 44, 45};
        case 41:
            return std::vector<int>{36, 45, 46};
        case 42:
            return std::vector<int>{37, 46};
        case 43:
            return std::vector<int>{38, 39, 47};
        case 44:
            return std::vector<int>{39, 40, 48};
        case 45:
            return std::vector<int>{40, 41, 49};
        case 46:
            return std::vector<int>{41, 42, 50};
        case 47:
            return std::vector<int>{43, 51};
        case 48:
            return std::vector<int>{44, 51, 52};
        case 49:
            return std::vector<int>{45, 52, 53};
        case 50:
            return std::vector<int>{46, 53};
        case 51:
            return std::vector<int>{47, 48};
        case 52:
            return std::vector<int>{48, 49};
        case 53:
            return std::vector<int>{49, 50};
        default:
            return std::vector<int>{};
    }
}

/*
 * Switch on vertex position to find adjacent tiles. If position not given, empty
 * vertex returned. Returns integer index of tile, not tile object or pointer.
 */
std::vector<int> Vertex::adjacentTile(int pos) {
    switch (pos) {
        case 0:
            return std::vector<int>{0};
        case 1:
            return std::vector<int>{1};
        case 2:
            return std::vector<int>{2};
        case 3:
            return std::vector<int>{0};
        case 4:
            return std::vector<int>{0, 1};
        case 5:
            return std::vector<int>{1, 2};
        case 6:
            return std::vector<int>{2};
        case 7:
            return std::vector<int>{0, 3};
        case 8:
            return std::vector<int>{0, 1, 4};
        case 9:
            return std::vector<int>{1, 2, 5};
        case 10:
            return std::vector<int>{2, 6};
        case 11:
            return std::vector<int>{3};
        case 12:
            return std::vector<int>{0, 3, 4};
        case 13:
            return std::vector<int>{1, 4, 5};
        case 14:
            return std::vector<int>{2, 5, 6};
        case 15:
            return std::vector<int>{6};
        case 16:
            return std::vector<int>{3, 7};
        case 17:
            return std::vector<int>{3, 4, 8};
        case 18:
            return std::vector<int>{4, 5, 9};
        case 19:
            return std::vector<int>{5, 6, 10};
        case 20:
            return std::vector<int>{6, 11};
        case 21:
            return std::vector<int>{7};
        case 22:
            return std::vector<int>{3, 7, 8};
        case 23:
            return std::vector<int>{4, 8, 9};
        case 24:
            return std::vector<int>{5, 9, 10};
        case 25:
            return std::vector<int>{6, 10, 11};
        case 26:
            return std::vector<int>{11};
        case 27:
            return std::vector<int>{7};
        case 28:
            return std::vector<int>{7, 8, 12};
        case 29:
            return std::vector<int>{8, 9, 13};
        case 30:
            return std::vector<int>{9, 10, 14};
        case 31:
            return std::vector<int>{10, 11, 15};
        case 32:
            return std::vector<int>{11};
        case 33:
            return std::vector<int>{7, 12};
        case 34:
            return std::vector<int>{8, 12, 13};
        case 35:
            return std::vector<int>{9, 13, 14};
        case 36:
            return std::vector<int>{10, 14, 15};
        case 37:
            return std::vector<int>{11, 15};
        case 38:
            return std::vector<int>{12};
        case 39:
            return std::vector<int>{12, 13, 16};
        case 40:
            return std::vector<int>{13, 14, 17};
        case 41:
            return std::vector<int>{14, 15, 18};
        case 42:
            return std::vector<int>{15};
        case 43:
            return std::vector<int>{12, 16};
        case 44:
            return std::vector<int>{13, 16, 17};
        case 45:
            return std::vector<int>{14, 17, 18};
        case 46:
            return std::vector<int>{15, 18};
        case 47:
            return std::vector<int>{16};
        case 48:
            return std::vector<int>{16, 17};
        case 49:
            return std::vector<int>{17, 18};
        case 50:
            return std::vector<int>{18};
        case 51:
            return std::vector<int>{16};
        case 52:
            return std::vector<int>{17};
        case 53:
            return std::vector<int>{18};
        default:
            return std::vector<int>{};
    }
}

bool Vertex::isAdjacent(int vertexPos) {
    return std::any_of(vertices.begin(), vertices.end(), [vertexPos](int i){return i == vertexPos;});
}
