#include <chrono>
#include <random>
#include <cstring>

#include "Board.h"

Board::Board(std::array<int, N_RESOURCE> &bit_board) {

    this->bitBoard = bit_board;
    // This gets set if the other constructor did not get called
    for (Tile*& t : tiles) t = new Tile();

    ports[0] = 0;
    portsRects[0] = new SDL_Rect;

    /*
     * bitBoard is array of location of each tile type with the first being desert, since all tiles
     * are already automatically deserts, we ignore the first row of the bit board and just focus
     * on the typed tiles
     */
    int j, k;
    for (const char& i : types) {
        j = bit_board[i];   // Get the bit board of that tile type

        k = 0;
        while (j) {         // While there are positive bits (i.e. locations of this tile type)

            // If there is a tile of this type at this position, set the current tile to that type
            if (j & 1) tiles[k]->type = i;

            // Move along the current tile (k) and the bit board of this tile type
            k++;
            j>>=1;
        }
    }

    int i = 0;
    for (Tile* tile : tiles) {
        if (tile->type == DESERT) {
            tile->hasRobber = true;
        } else {
            tile->chitNum = chitNums[i];
            i++;
        }
    }

    endTurnButton = new SDL_Rect;
    buildCityRect = new SDL_Rect;
    buildOptionsRect = new SDL_Rect;
    buildRoadRect = new SDL_Rect;
    buildSettlementRect = new SDL_Rect;
    buyDevelopmentCardRect = new SDL_Rect;
    confirmRect = new SDL_Rect;
    buildingRect = new SDL_Rect;
    rollDiceRect = new SDL_Rect;
    resourceMenuRect = new SDL_Rect;
    vpRect = new SDL_Rect;
    portsRect = new SDL_Rect;

    for (SDL_Texture* &t : diceRolls) t = nullptr;

    w = h = 0;

    // Creates an SDL window used to display pixel data to the user
    window = nullptr;

    /*
     * Creates an SDL renderer used to collate textures and then render them
     * simultaneously within the window with which it is associated
     */
    renderer = nullptr;

    endTurnTexture = buildCityTexture = buildSettlementTexture =
            buildRoadTexture = buyDevelopmentCardTexture = confirmTexture =
                    cityTexture = settlementTexture = building = devCardBack =
                            rollDice = resourceMenu = robberTexture = vpTexture = portsTexture = nullptr;
    SansLight = SansMed = SansBold = nullptr;

    buttonBuffer = 20;
    fontSize = 20;
    menuFraction = 4.0 / 5.0;
    minWidth = 900;
    minHeight = 400;
}

Board::Board() : Board(randBoard()) { }

std::array<int, N_RESOURCE> &Board::randBoard() {
    /*
     * Generates a random board by iterating through the number of tiles and choosing a random
     * tile to place there, checking first to see if any of those tiles are left to place.
     * Seed is generated at beginning so that it's essentially a random board every time.
     */

    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // This is essentially Python's randrange(6)
    std::uniform_int_distribution<int> distribution(0, 5);

    // Get copy of constant array of number of each type of resource
    std::array<int, N_RESOURCE> n_tiles{};
    std::copy(nTypes.begin(), nTypes.end(), n_tiles.begin());

    static std::array<int, N_RESOURCE> bit_board{0, 0, 0, 0, 0, 0};

    int i, t;
    for (i = 0; i < N_TILES; i++) {

        // Get tile that still has some left to be placed
        t = distribution(generator);
        while (!n_tiles[t]) t = ((t + 1) % N_RESOURCE);

        bit_board[t] |= (1 << i);
        n_tiles[t]--;
    }

    return bit_board;
}

bool Board::findTextures() {
    SDL_Surface *surface;
    for (const char& i : types) {
        switch (i) {
            case DESERT: surface = SDL_LoadBMP("resources/hexes/desert.bmp"); break;
            case FIELD: surface = SDL_LoadBMP("resources/hexes/field.bmp"); break;
            case FOREST: surface = SDL_LoadBMP("resources/hexes/forest.bmp"); break;
            case HILL: surface = SDL_LoadBMP("resources/hexes/hill.bmp"); break;
            case MOUNTAIN: surface = SDL_LoadBMP("resources/hexes/mountain.bmp"); break;
            case PASTURE: surface = SDL_LoadBMP("resources/hexes/pasture.bmp"); break;
            default: break;
        }
        if (surface == nullptr) {
            return false;
        } else {
            // Creates a surface (collection of pixels with all of their metadata)
            textures.insert(std::make_pair(i, SDL_CreateTextureFromSurface(renderer, surface)));
        }
    }

    // Also insert blank
    surface = SDL_LoadBMP("resources/hexes/blank.bmp");
    if (surface == nullptr) {
        return false;
    } else {
        textures.insert(std::make_pair(-1, SDL_CreateTextureFromSurface(renderer, surface)));
    }

    // And rest of buttons
    surface = SDL_LoadBMP("resources/misc/end_turn_2.bmp");
    if (surface == nullptr) {
        return false;
    } else {
        endTurnTexture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    surface = SDL_LoadBMP("resources/misc/building.bmp");
    if (surface == nullptr) {
        return false;
    } else {
        building = SDL_CreateTextureFromSurface(renderer, surface);
    }

    char buffer[100];
    for (int i = 0; i < 11; i++) {
        sprintf(buffer, "resources/dice/roll_%d.bmp", i + 2);
        if ((surface = SDL_LoadBMP(buffer)) == nullptr) return false;
        diceRolls[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }

    surface = SDL_LoadBMP("resources/cards/dev_back.bmp");
    if (surface == nullptr) return false;
    devCardBack = SDL_CreateTextureFromSurface(renderer, surface);

    surface = SDL_LoadBMP("resources/dice/dice_6.bmp");
    if (surface == nullptr) return false;
    rollDice = SDL_CreateTextureFromSurface(renderer, surface);

    surface = SDL_LoadBMP("resources/misc/resource_menu.bmp");
    if (surface == nullptr) return false;
    resourceMenu = SDL_CreateTextureFromSurface(renderer, surface);

    surface = SDL_LoadBMP("resources/misc/robber.bmp");
    if (surface == nullptr) return false;
    robberTexture = SDL_CreateTextureFromSurface(renderer, surface);

    surface = SDL_LoadBMP("resources/misc/victory_points.bmp");
    if (surface == nullptr) return false;
    vpTexture = SDL_CreateTextureFromSurface(renderer, surface);

    surface = SDL_LoadBMP("resources/ports/port_bridges_with_ports.bmp");
    if (surface == nullptr) return false;
    portsTexture = SDL_CreateTextureFromSurface(renderer, surface);

    return true;
}

bool Board::findFonts() {
    SansLight = TTF_OpenFont("fonts/OpenSans-Light.ttf", fontSize);
    SansMed = TTF_OpenFont("fonts/OpenSans-Medium.ttf", fontSize);
    SansBold = TTF_OpenFont("fonts/OpenSans-Bold.ttf", fontSize);
    if ((SansLight == nullptr) || (SansMed == nullptr) || (SansBold == nullptr)) {
        return false;
    } else {
        return true;
    }
}

bool Board::findChitNums() {
    char buffer[100];
    SDL_Surface *surface;
    for (Tile* tile : tiles) {
        if (tile->type == DESERT) continue;

        sprintf(buffer, "resources/chits/chit_%d.bmp", tile->chitNum);
        if ((surface = SDL_LoadBMP(buffer)) == nullptr) return false;
        tile->chitTexture = SDL_CreateTextureFromSurface(renderer, surface);
    }
    return true;
}

void Board::initializePorts() {

}

/*
 * Initialize menu, not currently used
 */
void Board::initializeMenu() {
    SDL_Surface *textSurface;
    SDL_Color White = {255, 255, 255, SDL_ALPHA_OPAQUE};
    int menuSize = 0;

    // All menu options for building

    textSurface = TTF_RenderText_Solid(SansMed, "Build Road", White);
    buildRoadTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    menuSize = std::max(textSurface->w, menuSize);

    buildRoadRect->w = textSurface->w;
    buildRoadRect->h = textSurface->h;

    textSurface = TTF_RenderText_Solid(SansMed, "Build Settlement", White);
    buildSettlementTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    menuSize = std::max(textSurface->w, menuSize);

    buildSettlementRect->w = textSurface->w;
    buildSettlementRect->h = textSurface->h;

    textSurface = TTF_RenderText_Solid(SansMed, "Build City", White);
    buildCityTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    menuSize = std::max(textSurface->w, menuSize);

    buildCityRect->w = textSurface->w;
    buildCityRect->h = textSurface->h;

    menuSize = std::max(textSurface->w, menuSize);

    textSurface = TTF_RenderText_Solid(SansMed, "Buy DevCard", White);
    buyDevelopmentCardTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    menuSize = std::max(textSurface->w, menuSize);

    buyDevelopmentCardRect->w = textSurface->w;
    buyDevelopmentCardRect->h = textSurface->h;

    textSurface = TTF_RenderText_Solid(SansMed, "Confirm", White);
    confirmTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    menuSize = std::max(textSurface->w, menuSize);

    confirmRect->w = textSurface->w;
    confirmRect->h = textSurface->h;

    buildOptionsRect->w = menuSize + (buttonBuffer * 2);
    buildOptionsRect->h = h - (2 * buttonBuffer);
}

void Board::initializeButtons() {
    // Text color
    SDL_Color White = {255, 255, 255, SDL_ALPHA_OPAQUE};

    // End turn text
    // SDL_Surface *surface = TTF_RenderText_Solid(SansMed, "END TURN", White);
    // endTurnTexture = SDL_CreateTextureFromSurface(renderer, surface);

    // End turn rect position
    endTurnButton->w = 200;
    endTurnButton->h = 140;

    buildingRect->w = 240;
    buildingRect->h = 160;

    rollDiceRect->w = rollDiceRect->h = 80;
    vpRect->w = vpRect->h = 300;

    SDL_Surface *surface = TTF_RenderText_Solid(SansMed, "S", White);
    settlementTexture = SDL_CreateTextureFromSurface(renderer, surface);

    surface = TTF_RenderText_Solid(SansMed, "C", White);
    cityTexture = SDL_CreateTextureFromSurface(renderer, surface);

    resizeButtons();
}

bool Board::initializeWindowAndRenderer() {
    SDL_DisplayMode DM;
    SDL_GetDesktopDisplayMode(0, &DM);

    // Sets SDL-window to be resizable
    if (SDL_CreateWindowAndRenderer((int)(0.9 * DM.w), (int)(0.9 * DM.h), SDL_WINDOW_RESIZABLE,
                                    &window, &renderer)) {
        return false;
    } else {
        // Set blend mode to none
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetWindowMinimumSize(window, minWidth, minHeight);

        return true;
    }
}

bool Board::initialize() {
    // Load textures into board->textures array
    if (!findTextures()) return false;

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Get positions for all tiles
    setSize(windowWidth, windowHeight);

    // Get size of tile
    int half_tile = tiles[0]->tileRect->w / 2;
    int tile_side = tiles[0]->tileRect->h / 2;

    // Find location of first (top level vertex, left side of row)
    std::pair<int, int> vertex0{tiles[0]->x, tiles[0]->y - tile_side};

    // Make new vertex
    vertices[0] = new Vertex(vertex0);

    // Find adjacent vertices and tiles
    for (int const& j : Vertex::adjacentVertex(0)) vertices[0]->vertices.push_back(j);
    for (int const& j : Vertex::adjacentTile(0)) vertices[0]->tiles.push_back(j);

    // Set most recent vertex to first one, then iterate through rest, resetting recent each time
    std::pair<int, int> recent = vertex0;
    for (int i = 1; i < N_VERTICES; i++) {
        recent = Vertex::position(half_tile, tile_side, vertex0, recent, i);
        vertices[i] = new Vertex(recent);

        // Each time, perform the same copy over of adjacent tiles and vertices
        for (int const& j : Vertex::adjacentVertex(i)) vertices[i]->vertices.push_back(j);
        for (int const& j : Vertex::adjacentTile(i)) vertices[i]->tiles.push_back(j);
    }

    for (int i = 0; i < N_TILES; i++) {
        for (int const& j : Tile::adjacentVertices(i)) tiles[i]->vertices.push_back(j);
    }

    if (!findChitNums()) return false;

    return true;
}

void Board::setSize(int windowWidth, int windowHeight) {
    w = windowWidth;
    h = windowHeight;

    int size = std::min(w, h);
    buttonBuffer = size / 24;
    int tile_side = (int)(size / (6 * sqrt(3)));
    std::pair<int, int> center = std::make_pair(w / 2, h / 2);

    std::pair<int, int> hex_coords;

    for (int i = 0; i < N_TILES; i++) {
        hex_coords = Tile::position(i, center, size / 12, tile_side);
        tiles[i]->setRect(hex_coords.first, hex_coords.second, size / 6, 2 * tile_side);
    }
}

void Board::resize(int windowWidth, int windowHeight) {
    setSize(windowWidth, windowHeight);
    resizeButtons();
    resizeVertices();
}

// Resize menu with buttons, not used
void Board::resizeMenu() const {
    buildOptionsRect->x = (int)(w * menuFraction) - buttonBuffer;
    buildOptionsRect->y = buttonBuffer;
    buildOptionsRect->h = h - (2 * buttonBuffer);

    buildRoadRect->x = buildOptionsRect->x + ((buildOptionsRect->w - buildRoadRect->w) / 2);
    buildRoadRect->y = buildOptionsRect->y + buttonBuffer;

    buildSettlementRect->x = buildOptionsRect->x + ((buildOptionsRect->w - buildSettlementRect->w) / 2);
    buildSettlementRect->y = buildRoadRect->y + buildRoadRect->h + (2 * buttonBuffer);

    buildCityRect->x = buildOptionsRect->x + ((buildOptionsRect->w - buildCityRect->w) / 2);
    buildCityRect->y = buildSettlementRect->y + buildSettlementRect->h + (2 * buttonBuffer);

    buyDevelopmentCardRect->x = buildOptionsRect->x + ((buildOptionsRect->w - buyDevelopmentCardRect->w) / 2);
    buyDevelopmentCardRect->y = buildCityRect->y + buildCityRect->h + (2 * buttonBuffer);

    confirmRect->x = buildOptionsRect->x + ((buildOptionsRect->w - confirmRect->w) / 2);
    confirmRect->y = buyDevelopmentCardRect->y + buyDevelopmentCardRect->h + (2 * buttonBuffer);
}

void Board::resizeButtons() const {
    buildingRect->x = 0;
    buildingRect->y = 0;

    endTurnButton->x = 0;
    endTurnButton->y = h - (endTurnButton->h + buttonBuffer);

    rollDiceRect->x = w - buttonBuffer - rollDiceRect->w;
    rollDiceRect->y = h - buttonBuffer - rollDiceRect->h;

    vpRect->y = rollDiceRect->y - 120;
    vpRect->x = rollDiceRect->x - vpRect->w;

    resourceMenuRect->w = (w / 8) + 100;
    resourceMenuRect->h = h / 3;
    resourceMenuRect->x = (5 * w) / 6;
    resourceMenuRect->y = buttonBuffer;
}

/*
 * Resizes vertices based on tile positions. Make sure to call Board::resize() before this if the
 * window size has changed.
 */
void Board::resizeVertices() const {
    int half_tile = tiles[0]->tileRect->w / 2;
    int tile_side = tiles[0]->tileRect->h / 2;

    std::pair<int, int> vertex0{tiles[0]->x, tiles[0]->y - tile_side};
    vertices[0]->x = vertex0.first;
    vertices[0]->y = vertex0.second;

    std::pair<int, int> recent = vertex0;

    for (int i = 1; i < N_VERTICES; i++) {
        recent = Vertex::position(half_tile, tile_side, vertex0, recent, i);
        vertices[i]->x = recent.first;
        vertices[i]->y = recent.second;
    }
}

void Board::drawAdjacent(Vertex &vertex, Player &player) {
    // Draw square at adjacent vertex -- for testing
    for (int const& i : vertex.vertices) {
        printf("%d; ", i);
        SDL_Rect adj_vert;
        adj_vert.x = vertices[i]->x - Vertex::s_vert_size / 2;
        adj_vert.y = vertices[i]->y - Vertex::s_vert_size / 2;
        adj_vert.w = adj_vert.h = Vertex::s_vert_size;

        SDL_SetRenderDrawColor(renderer, player.R, player.G, player.B, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &adj_vert);
    }

    // Draw square at adjacent tile -- for testing
    for (int const& i : vertex.tiles) {
        SDL_Rect adj_tile;
        adj_tile.x = tiles[i]->x - Vertex::s_vert_size / 2;
        adj_tile.y = tiles[i]->y - Vertex::s_vert_size / 2;
        adj_tile.w = adj_tile.h = Vertex::s_vert_size;

        SDL_SetRenderDrawColor(renderer, player.R, player.G, player.B, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &adj_tile);
    }
}

/*
 * Unused function, moved to keypresses for actions instead of buttons
 */
void Board::drawMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderFillRect(renderer, buildOptionsRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawRect(renderer, buildRoadRect);
    SDL_RenderCopy(renderer, buildRoadTexture, nullptr, buildRoadRect);

    SDL_RenderDrawRect(renderer, buildSettlementRect);
    SDL_RenderCopy(renderer, buildSettlementTexture, nullptr, buildSettlementRect);

    SDL_RenderDrawRect(renderer, buildCityRect);
    SDL_RenderCopy(renderer, buildCityTexture, nullptr, buildCityRect);

    SDL_RenderDrawRect(renderer, buyDevelopmentCardRect);
    SDL_RenderCopy(renderer, buyDevelopmentCardTexture, nullptr, buyDevelopmentCardRect);

    SDL_RenderDrawRect(renderer, confirmRect);
    SDL_RenderCopy(renderer, confirmTexture, nullptr, confirmRect);
}

void Board::drawResources(Player &player) {
    int x = resourceMenuRect->x + (resourceMenuRect->w / 4);
    int incrementY = resourceMenuRect->h / 5;

    SDL_Color gray = {80, 80, 80, SDL_ALPHA_OPAQUE};

    SDL_Surface *surface;
    SDL_Texture *texture;
    char buffer[20];
    int y = incrementY;
    for (const int& i : {HILL, FIELD, PASTURE, MOUNTAIN, FOREST}) {
        sprintf(buffer, "%d", player.resources[i]);
        surface = TTF_RenderText_Solid(SansMed, buffer, gray);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect{x, y - (incrementY / 3), resourceMenuRect->w / 10, incrementY / 2};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_RenderDrawRect(renderer, &rect);
        y += incrementY;
    }

    x = vpRect->x + (vpRect->w / 2);
    y = vpRect->y + 140;
    sprintf(buffer, "%d", player.victoryPoints);
    surface = TTF_RenderText_Solid(SansMed, buffer, gray);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect vpCount{x, y, 30, 40};
    SDL_RenderCopy(renderer, texture, nullptr, &vpCount);
    SDL_RenderDrawRect(renderer, &vpCount);
}

void Board::drawButtons() {
    // Draw turn button here
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);


    // SDL_RenderFillRect(renderer, endTurnButton);
    // SDL_RenderCopy(renderer, endTurnTexture, nullptr, endTurnButton);
    SDL_RenderCopy(renderer, endTurnTexture, nullptr, endTurnButton);
    SDL_RenderDrawRect(renderer, endTurnButton);

    SDL_RenderCopy(renderer, building, nullptr, buildingRect);
    SDL_RenderDrawRect(renderer, buildingRect);

    SDL_RenderCopy(renderer, rollDice, nullptr, rollDiceRect);
    SDL_RenderDrawRect(renderer, rollDiceRect);

    SDL_RenderCopy(renderer, resourceMenu, nullptr, resourceMenuRect);
    SDL_RenderDrawRect(renderer, resourceMenuRect);

    SDL_RenderCopy(renderer, vpTexture, nullptr, vpRect);
    SDL_RenderDrawRect(renderer, vpRect);
}

void Board::drawRoll(int roll) {
    SDL_Texture *rollTexture = diceRolls[roll - 2];
    SDL_Rect rollRect{buildingRect->x,
                      buildingRect->y + buildingRect->h - (2 * buttonBuffer),
                      buildingRect->w, buildingRect->h};
    SDL_RenderCopy(renderer, rollTexture, nullptr, &rollRect);
}

void Board::drawActiveBuilding(SDL_Texture *texture) const {
    SDL_Rect rect{buildingRect->x + 155,
                  buildingRect->y + 65, 40, 40};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Board::drawRoad(SDL_Point &first, SDL_Point &second, Player *player) const {
    SDL_Texture *road;
    int diff_x = std::abs(first.x - second.x);

    auto *roadRect = new SDL_Rect;
    int adjust = std::min(w, h) / 24;
    if (diff_x < 10) {
        road = player->roadTextures[0];
        roadRect->x = first.x - adjust;
        roadRect->w = adjust * 2;
        roadRect->h = std::abs(first.y - second.y);
        roadRect->y = std::min(first.y, second.y);
    } else {
        double slope = (first.y - second.y) / (double) (first.x - second.x);
        if (slope < 0) {
            road = player->roadTextures[1];
        } else {
            road = player->roadTextures[2];
        }
        roadRect->x = std::min(first.x, second.x);
        roadRect->w = std::abs(first.x - second.x);
        roadRect->h = std::abs(first.y - second.y) + adjust;
        roadRect->y = std::min(first.y, second.y) - (adjust / 2);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
    SDL_RenderCopy(renderer, road, nullptr, roadRect);
    SDL_RenderDrawRect(renderer, roadRect);

}

void Board::drawPlayerProperties(const std::vector<Player*>& players) {
    Vertex *vertex;
    int adjust = std::min(w, h) / 28;
    for (Player* const& player : players) {
        SDL_SetRenderDrawColor(renderer, player->R, player->G, player->B, SDL_ALPHA_OPAQUE);
        for (auto const& point : player->roads) {
            SDL_Point first{vertices[point.first]->x, vertices[point.first]->y};
            SDL_Point second{vertices[point.second]->x, vertices[point.second]->y};
            drawRoad(first, second, player);
        }

        SDL_SetRenderDrawColor(renderer, player->R, player->G, player->B, SDL_ALPHA_TRANSPARENT);
        for (int &i : player->buildings) {
            vertex = vertices[i];

            // Draw background
            SDL_Rect rect{vertex->x - adjust,
                          vertex->y - adjust,
                          adjust * 2,
                          adjust * 2};

            if (vertex->hasSettlement()) {
                SDL_RenderCopy(renderer, player->settlementTexture, nullptr, &rect);
            } else if (vertex->hasCity()) {
                SDL_RenderCopy(renderer, player->cityTexture, nullptr, &rect);
            }

            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}

void Board::draw() {

//    portsRect->h = std::min(h, w);
//    portsRect->w = portsRect->h * 2;
//    portsRect->y = 0;
//    portsRect->x = portsRect->w / 11;
//    SDL_RenderCopy(renderer, portsTexture, nullptr, portsRect);
//    SDL_RenderDrawRect(renderer, portsRect);

    int chitSize = w / 18;
    for (Tile* const& tile : tiles) {
        /*
         * Copies the tile texture to the renderer at the correct destination
         * position specified in hex_pos using the entire source texture
         * (hence the third NULL argument)
         */
        SDL_RenderCopy(renderer, textures.at(tile->type), nullptr, tile->tileRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_Rect chit{tile->x - chitSize, tile->y - chitSize, 2 * chitSize, 2 * chitSize};
        SDL_RenderCopy(renderer, tile->chitTexture, nullptr, &chit);
        SDL_RenderDrawRect(renderer, &chit);

        if (tile->hasRobber) {
            SDL_Rect robberRect{tile->x - (chitSize / 2), tile->y - (chitSize / 2), chitSize, chitSize};
            SDL_RenderCopy(renderer, robberTexture, nullptr, &robberRect);
            SDL_RenderDrawRect(renderer, &robberRect);
        }
    }

    for (Vertex* const& vertex : vertices) {
        SDL_Rect vert_rect;
        vert_rect.w = vert_rect.h = 10;
        vert_rect.x = vertex->x - 5;
        vert_rect.y = vertex->y - 5;

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &vert_rect);
    }

    drawButtons();
}

void Board::freeBoard() {

    // Destroy all textures
    for (auto const& value : textures) {
        SDL_DestroyTexture(value.second);
    }

    // Free all tiles and SDL_Rects
    for (Tile* const& tile : tiles) {
        delete tile->tileRect;
        delete tile;
    }

    for (Vertex* const& vertex : vertices) delete vertex;

    SDL_DestroyTexture(endTurnTexture);
    delete endTurnButton;
    TTF_CloseFont(SansLight);
    TTF_CloseFont(SansMed);
    TTF_CloseFont(SansBold);
}

std::array<int, N_RESOURCE> Board::getBitBoard() const {
    return bitBoard;
}
