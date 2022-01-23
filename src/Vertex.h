#ifndef VERTEX_H
#define VERTEX_H

#include "Tile.h"
#include <vector>


/*
 * Vertex is a position where a player can build a settlement/city and touches at least one tile
 * but up to three tiles.
 */

class Vertex {

private:
    char building;      // Type of building at vertex; 0 = None; 1 = settlement; 2 = city

    // Vertex can either be created with x, y integers or pair(x, y)
public:

    static int s_vert_size;

    int x, y;
    std::vector<int> vertices;      // list of other vertices connected i.e. places you can build roads to
    std::vector<int> tiles;         // list of adjacent tiles i.e. places to collect resources from
    char port;          // Port value is equal to the given port resource, -1 if 3-for-1, 0 if no port

    Vertex(std::pair<int, int> coordinates, char port);
    Vertex(int x, int y, char port);
    Vertex(int x, int y);
    explicit Vertex(char port);
    explicit Vertex(std::pair<int, int> coordinates);
    Vertex();

    bool in(int _x, int _y) const;

    // Getters for building type
    bool isEmpty() const;
    bool hasSettlement() const;
    bool hasCity() const;

    // Setters for building type
    void buildSettlement();
    void buildCity();

    int buildingValue() const;

    // Return resources
    std::array<int, N_RESOURCE> force(std::array<Tile*, N_TILES> boardTiles) const;

    static std::pair<int, int> position(int half_tile, int tile_side, std::pair<int, int> top_left,
                                        std::pair<int, int> recent, int pos);

    static std::vector<int> adjacentVertex(int pos);
    static std::vector<int> adjacentTile(int pos);
    bool isAdjacent(int vertexPos);
};


#endif //VERTEX_H
