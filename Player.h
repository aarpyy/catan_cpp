#ifndef PLAYER_H
#define PLAYER_H

#include "Vertex.h"

class Player {
public:
    std::vector<char> developmentCards{};
    std::array<int, N_RESOURCE> resources{};  // Array of resources (index = resource type - 1; value = # of resource)
    std::vector<int> buildings;
    std::array<SDL_Texture*, 3> roadTextures{};

    /*
     * Vector of all the vertices a player has access to via roads. These ignore buildings, as they
     * are simply an indicator of if a road belonging to this player touches a vertex or not.
     */
    std::vector<std::pair<int, int>> roads;
    int id;
    int nSettlements, nCities, nRoads, victoryPoints;
    bool firstSettlement, secondSettlement, firstRoad, secondRoad;
    Uint8 R, G, B;
    SDL_Cursor *cursor;
    SDL_Texture *settlementTexture, *cityTexture;

    Player(int _id, Uint8 r, Uint8 g, Uint8 b);
    Player(int _id, std::array<Uint8, 3> color);
    explicit Player(int _id);

    bool loadTextures(SDL_Renderer *renderer);

    bool hasBuilding(int vertIndex);
    bool setupRoad(int vertSrc, int vertDest);
    bool setupSettlement(int vertIndex);
    bool buildRoad(int vertSrc, int vertDest);
    bool buildSettlement(int vertIndex);
    bool buildCity();
    bool buyDevelopmentCard();

    void collect(std::array<Vertex*, N_VERTICES> vertices, std::array<Tile*, N_TILES> tiles);
};


#endif //PLAYER_H
