#include "Player.h"

#include <utility>
#include <algorithm>

Player::Player(int _id, Uint8 r, Uint8 g, Uint8 b) {
    R = r;
    G = g;
    B = b;
    nSettlements = N_SETTLEMENTS;
    nCities = N_CITIES;
    nRoads = N_ROADS;
    victoryPoints = 0;
    id = _id + 1;
    firstSettlement = secondSettlement = firstRoad = secondRoad = false;

    for (int &i : resources) i = 0;

    char buffer[100];
    sprintf(buffer, "resources/cursors/cursor_%d.bmp", id);
    SDL_Surface *surface = SDL_LoadBMP(buffer);
    if (surface == nullptr) {
        cursor = SDL_GetDefaultCursor();
    } else {
        cursor = SDL_CreateColorCursor(surface, 0, 0);
    }

    for (SDL_Texture* & texture : roadTextures) texture = nullptr;
    settlementTexture = nullptr;
    cityTexture = nullptr;
}

Player::Player(int _id, std::array<Uint8, 3> color) : Player(_id, color[0], color[1], color[2]) { }

Player::Player(int _id) : Player(_id, 255, 0, 0) { }

bool Player::loadTextures(SDL_Renderer *renderer) {
    char buffer[100];
    SDL_Surface *surface;
    for (int i = 0; i < 3; i++) {
        sprintf(buffer, "resources/roads/road_%d_angle_%d.bmp", id, i + 1);
        if ((surface = SDL_LoadBMP(buffer)) == nullptr) return false;
        roadTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }

    // Load settlement texture
    sprintf(buffer, "resources/buildings/settlement_%d.bmp", id);
    if ((surface = SDL_LoadBMP(buffer)) == nullptr) return false;
    settlementTexture = SDL_CreateTextureFromSurface(renderer, surface);

    // Load city texture
    sprintf(buffer, "resources/buildings/city_%d.bmp", id);
    if ((surface = SDL_LoadBMP(buffer)) == nullptr) return false;
    cityTexture = SDL_CreateTextureFromSurface(renderer, surface);

    return true;
}

bool Player::hasBuilding(int vertIndex) {
    for (int &i : buildings) {
        if (i == vertIndex) return true;
    }
    return false;
}

bool Player::setupRoad(int vertSrc, int vertDest) {
    nRoads--;
    roads.emplace_back(vertSrc, vertDest);
    roads.emplace_back(vertDest, vertSrc);
    if (firstRoad) secondRoad = true;
    else firstRoad = true;
    return true;
}

bool Player::setupSettlement(int vertIndex) {
    nSettlements--;
    victoryPoints++;
    buildings.push_back(vertIndex);
    if (firstSettlement) secondSettlement = true;
    else firstSettlement = true;
    return true;
}

bool Player::buildRoad(int vertSrc, int vertDest) {
    if (!nRoads) {
        return false;
    } else {
        for (char const& i : types) {
            if (resources[i] < roadRecipe[i]) return false;
        }
        for (char const& i : types) {
            resources[i] -= roadRecipe[i];
        }

        nRoads--;

        roads.emplace_back(vertSrc, vertDest);
        roads.emplace_back(vertDest, vertSrc);
        return true;
    }
}

/*
 * Builds a settlement at the given Vertex.
 *
 * Returns 1 if player does not have enough settlements. Otherwise,
 * it builds the settlement and returns 0.
 *
 * Returns 2 if Vertex has something built there.
 */
bool Player::buildSettlement(int vertIndex) {
    // If player doesn't have enough settlements
    if (!nSettlements) {
        return false;
    } else {
        // Check to see if they have the resources
        for (char const& i : types) {
            if (resources[i] < settlementRecipe[i]) return false;
        }

        // If we get here, they have the resources so make the update to resource numbers
        for (char const& i : types) {
            resources[i] -= settlementRecipe[i];
        }

        victoryPoints++;
        nSettlements--;
        buildings.push_back(vertIndex);
        return true;
    }
}

/*
 * Checks to see if player can upgrade a settlement to a city, updating player's city, settlement,
 * and resource values if they do.
 */
bool Player::buildCity() {
    if (!nCities) {
        return false;
    } else {
        // Check to see if they have the resources
        for (char const& i : types) {
            if (resources[i] < cityRecipe[i]) return false;
        }

        // If we get here, they have the resources so make the update to resource numbers
        for (char const& i : types) {
            resources[i] -= cityRecipe[i];
        }
        victoryPoints += 2;
        nCities--;
        nSettlements++;
        return true;
    }
}

bool Player::buyDevelopmentCard() {
    for (char const& i : types) {
        if (resources[i] < developmentCardRecipe[i]) return false;
    }

    for (char const& i : types) {
        resources[i] -= developmentCardRecipe[i];
    }

    return true;

}

void Player::collect(std::array<Vertex*, N_VERTICES> vertices, std::array<Tile*, N_TILES> tiles) {
    std::array<int, N_RESOURCE> vertexResources{};
    for (Vertex* const& vertex : vertices) {
        vertexResources = vertex->force(tiles);
        for (const char& i : types) resources[i] = vertexResources[i];
    }
}
